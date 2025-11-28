#include "BTJsonSerializer.h"
#include "JsonObjectConverter.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTService.h"
#include "UObject/SavePackage.h"

FBTNodeJSON UBTJsonSerializer::SerializeNode(UBTNode* Node)
{
    FBTNodeJSON Out;
    if (!Node) return Out;

    Out.NodeClass = Node->GetClass()->GetPathName();
    Out.NodeName = Node->NodeName;

    // Serialize properties
    for (TFieldIterator<FProperty> It(Node->GetClass()); It; ++It)
    {
        FProperty* Prop = *It;
        FString Value;
        Prop->ExportTextItem_Direct(Value,
            Prop->ContainerPtrToValuePtr<void>(Node),
            nullptr,
            Node, PPF_None);
        Out.Properties.Add(Prop->GetName(), Value);
    }

    // Composite children
    if (UBTCompositeNode* Composite = Cast<UBTCompositeNode>(Node))
    {
        // Children
        for (const FBTCompositeChild& Child : Composite->Children)
        {
            if (Child.ChildComposite)
                Out.Children.Add(SerializeNode(Cast<UBTNode>(Child.ChildComposite)));
            else if (Child.ChildTask)
                Out.Children.Add(SerializeNode(Cast<UBTNode>(Child.ChildTask)));

            // Decorators
            for (UBTDecorator* Deco : Child.Decorators)
                Out.Decorators.Add(SerializeNode(Cast<UBTNode>(Deco)));
        }

        // Services
        for (UBTService* Srv : Composite->Services)
            Out.Services.Add(SerializeNode(Cast<UBTNode>(Srv)));
    }

    return Out;
}

bool UBTJsonSerializer::SerializeBehaviorTreeToJSON(UBehaviorTree* BT, FString& OutJSON)
{
    if (!BT || !BT->RootNode)
        return false;

    FBTNodeJSON RootData = SerializeNode(BT->RootNode);
    return FJsonObjectConverter::UStructToJsonObjectString(RootData, OutJSON);
}

UBTNode* UBTJsonSerializer::CreateNodeFromJSON(const FBTNodeJSON& Data, UObject* Outer)
{
    UClass* NodeClass = LoadObject<UClass>(nullptr, *Data.NodeClass);
    if (!NodeClass) return nullptr;

    UBTNode* Node = NewObject<UBTNode>(Outer, NodeClass, NAME_None, RF_Public | RF_Standalone);
    Node->NodeName = Data.NodeName;

    // Restore properties
    for (const auto& Pair : Data.Properties)
    {
        FProperty* Prop = NodeClass->FindPropertyByName(*Pair.Key);
        if (Prop)
        {
            Prop->ImportText_Direct(*Pair.Value,
                Prop->ContainerPtrToValuePtr<void>(Node),
                Node, PPF_None);
        }
    }

    return Node;
}

UBehaviorTree* UBTJsonSerializer::DeserializeJSONToBehaviorTree(
    const FString& JSON,
    const FString& PackagePath,
    const FString& AssetName)
{
    FBTNodeJSON RootJSON;
    if (!FJsonObjectConverter::JsonObjectStringToUStruct(JSON, &RootJSON))
        return nullptr;

    // Create package
    UPackage* Package = CreatePackage(*PackagePath);
    Package->FullyLoad();

    // Create asset
    UBehaviorTree* BT = NewObject<UBehaviorTree>(Package, *AssetName,
        RF_Public | RF_Standalone | RF_MarkAsRootSet);

    // Root
    BT->RootNode = Cast<UBTCompositeNode>(CreateNodeFromJSON(RootJSON, BT));

    // Recursively rebuild
    TFunction<void(UBTNode*, const FBTNodeJSON&)> Rebuild =
        [&](UBTNode* Node, const FBTNodeJSON& Data)
    {
        if (UBTCompositeNode* Composite = Cast<UBTCompositeNode>(Node))
        {
            // Children
            for (const FBTNodeJSON& ChildData : Data.Children)
            {
                UBTNode* ChildNode = CreateNodeFromJSON(ChildData, BT);

                FBTCompositeChild NewChild;
                NewChild.ChildTask = Cast<UBTTaskNode>(ChildNode);
                NewChild.ChildComposite = Cast<UBTCompositeNode>(ChildNode);

                Composite->Children.Add(NewChild);

                Rebuild(ChildNode, ChildData);
            }

            // Decorators
            if (Composite->Children.Num() > 0)
            {
                FBTCompositeChild& LastChild = Composite->Children.Last();
                for (const FBTNodeJSON& DecoData : Data.Decorators)
                {
                    UBTNode* Deco = CreateNodeFromJSON(DecoData, BT);
                    LastChild.Decorators.Add(Cast<UBTDecorator>(Deco));
                }
            }

            // Services
            for (const FBTNodeJSON& ServiceData : Data.Services)
            {
                UBTNode* SrvNode = CreateNodeFromJSON(ServiceData, BT);
                Composite->Services.Add(Cast<UBTService>(SrvNode));
            }
        }
    };

    Rebuild(BT->RootNode, RootJSON);

    BT->MarkPackageDirty();

    // UE 5.6 SavePackage format
    FString Filename = FPackageName::LongPackageNameToFilename(
        PackagePath, FPackageName::GetAssetPackageExtension());

    FSavePackageArgs SaveArgs;
    SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;

    UPackage::SavePackage(Package, BT, *Filename, SaveArgs);

    return BT;
}


bool IsValidBehaviorTree(UBehaviorTree* BT)
{
	if (!BT)
		return false;

	if (!BT->RootNode)
		return false;

	if (!BT->RootNode->IsA<UBTCompositeNode>())
		return false;

	return true;
}
