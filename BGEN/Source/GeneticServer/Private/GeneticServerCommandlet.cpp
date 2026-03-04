#include "GeneticServerCommandlet.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "Serialization/JsonSerializer.h"
#include "JsonObjectConverter.h"
#include "Containers/Ticker.h"
#include "Async/TaskGraphInterfaces.h"

DEFINE_LOG_CATEGORY_STATIC(LogGeneticServer, Log, All);

UGeneticServerCommandlet::UGeneticServerCommandlet()
{
    // These flags tell the engine NOT to load worlds or standard game features
    IsClient = false;
    IsEditor = true;
    IsServer = false;
    LogToConsole = true;
}

int32 UGeneticServerCommandlet::Main(const FString& Params)
{
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));
    UE_LOG(LogGeneticServer, Warning, TEXT(" Starting Headless Genetic Server Commandlet... "));
    UE_LOG(LogGeneticServer, Warning, TEXT("================================================"));

    // 1. Initialize HTTP Server
    FModuleManager::LoadModuleChecked<FHttpServerModule>("HttpServer");
    auto& HttpServerModule = FHttpServerModule::Get();
    
    uint32 Port = 8080;
    TSharedPtr<IHttpRouter> HttpRouter = HttpServerModule.GetHttpRouter(Port);

    if (!HttpRouter.IsValid())
    {
        UE_LOG(LogGeneticServer, Error, TEXT("Failed to initialize HTTP Router on port %d"), Port);
        return 1;
    }

    // 2. BIND ROUTES
    HttpRouter->BindRoute(FHttpPath(TEXT("/api/check")), EHttpServerRequestVerbs::VERB_GET, 
        FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString HashValue;
            float FitnessResult = -1.0f; 

            if (const FString* HashParam = Request.QueryParams.Find(TEXT("hash")))
            {
                HashValue = *HashParam;
                if (float* FoundFitness = GlobalEvaluatedTrees.Find(HashValue))
                {
                    FitnessResult = *FoundFitness;
                }
            }

            TSharedPtr<FJsonObject> JsonResponse = MakeShareable(new FJsonObject());
            JsonResponse->SetNumberField(TEXT("fitness"), FitnessResult);

            FString ResponseString;
            TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResponseString);
            FJsonSerializer::Serialize(JsonResponse.ToSharedRef(), Writer);

            TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(ResponseString, FString(TEXT("application/json")));
            OnComplete(MoveTemp(Response));
            return true;
        }));

    HttpRouter->BindRoute(FHttpPath(TEXT("/api/submit")), EHttpServerRequestVerbs::VERB_POST, 
        FHttpRequestHandler::CreateLambda([this](const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
        {
            FString BodyString(Request.Body.Num(), UTF8_TO_TCHAR((const char*)Request.Body.GetData()));
            TSharedPtr<FJsonObject> JsonObject;
            TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(BodyString);

            if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
            {
                FString HashValue = JsonObject->GetStringField(TEXT("hash"));
                float FitnessValue = JsonObject->GetNumberField(TEXT("fitness"));
                GlobalEvaluatedTrees.Add(HashValue, FitnessValue);
                UE_LOG(LogGeneticServer, Log, TEXT("Received new tree: %s with fitness %f"), *HashValue, FitnessValue);
            }

            TUniquePtr<FHttpServerResponse> Response = FHttpServerResponse::Create(FString(TEXT("{\"status\":\"success\"}")), TEXT("application/json"));
            OnComplete(MoveTemp(Response));
            return true;
        }));

    HttpServerModule.StartAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server listening on port %d"), Port);

    // 3. INFINITE LOOP TO KEEP PROCESS ALIVE
    double LastTime = FPlatformTime::Seconds();
    while (!IsEngineExitRequested())
    {
        // Sleep for 10ms to prevent 100% CPU usage
        FPlatformProcess::Sleep(0.01f); 

        double CurrentTime = FPlatformTime::Seconds();
        float DeltaTime = CurrentTime - LastTime;
        LastTime = CurrentTime;

        // Tick core engine systems required for async HTTP thread callbacks
        FTSTicker::GetCoreTicker().Tick(DeltaTime);
        FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
    }

    // 4. CLEANUP ON SHUTDOWN (Ctrl+C)
    HttpServerModule.StopAllListeners();
    UE_LOG(LogGeneticServer, Warning, TEXT("Genetic Central Server Shutdown."));

    return 0;
}