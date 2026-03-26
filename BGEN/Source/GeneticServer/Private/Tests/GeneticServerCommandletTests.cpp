#include "Misc/AutomationTest.h"
#include "GeneticServerCommandlet.h"

// The EAutomationTestFlags::EditorContext ensures this runs in the Editor where Commandlets exist.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGeneticServerJobQueueTest, "GeneticGeneration.Server.JobQueue", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FGeneticServerJobQueueTest::RunTest(const FString& Parameters)
{
	// 1. Arrange: Instantiate the object dynamically. 
	// We do not provide an Outer, so it belongs to the transient package and is garbage collected automatically.
	UGeneticServerCommandlet* ServerCommandlet = NewObject<UGeneticServerCommandlet>();
	
	if (!TestNotNull(TEXT("Server Commandlet instantiated successfully"), ServerCommandlet))
	{
		return false;
	}

	// 2. Act: Populate the private queue
	ServerCommandlet->JobQueue.Add(TEXT("/Game/Genetics/Job_1"));
	ServerCommandlet->JobQueue.Add(TEXT("/Game/Genetics/Job_2"));

	// 3. Assert
	TestEqual(TEXT("Job queue starts with exactly 2 items"), ServerCommandlet->JobQueue.Num(), 2);

	FString FirstJob = ServerCommandlet->GetNextJob();
	TestEqual(TEXT("GetNextJob follows FIFO order (Job 1)"), FirstJob, TEXT("/Game/Genetics/Job_1"));
	TestEqual(TEXT("Queue decrements correctly"), ServerCommandlet->JobQueue.Num(), 1);

	FString SecondJob = ServerCommandlet->GetNextJob();
	TestEqual(TEXT("GetNextJob follows FIFO order (Job 2)"), SecondJob, TEXT("/Game/Genetics/Job_2"));
	TestEqual(TEXT("Queue is empty"), ServerCommandlet->JobQueue.Num(), 0);

	FString EmptyJob = ServerCommandlet->GetNextJob();
	TestTrue(TEXT("GetNextJob returns empty string when queue is exhausted"), EmptyJob.IsEmpty());

	return true;
}