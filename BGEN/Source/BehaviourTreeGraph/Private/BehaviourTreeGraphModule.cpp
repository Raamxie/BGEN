#include "BehaviourTreeGraphModule.h"

#include "BehaviorTreeGraph.h"
#include "BehaviorTreeGraphNode_Composite.h"
#include "BehaviorTreeGraphNode_Root.h"
#include "BehaviorTreeGraphNode_Task.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Modules/ModuleManager.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor.h"
#include "EditorFramework/AssetImportData.h"
#include "EdGraphUtilities.h"

#include "UObject/ObjectMacros.h"
#include "UObject/GCObjectScopeGuard.h"


IMPLEMENT_MODULE(FBehaviourTreeGraphModule, BehaviourTreeGraph)

void FBehaviourTreeGraphModule::StartupModule()
{
    // No auto-binding — manual run
	UE_LOG(LogTemp, Error, TEXT("BTG loaded"));
}

void FBehaviourTreeGraphModule::ShutdownModule()
{
}

void FBehaviourTreeGraphModule::BuildGraphForBehaviorTree(const FString& AssetPackagePath)
{
    if (!GIsEditor)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildGraphForBehaviorTree: Not running in Editor"));
        return;
    }

    FString ObjectPath = AssetPackagePath;
    if (!AssetPackagePath.Contains(TEXT(".")))
    {
        FString AssetName = FPackageName::GetShortName(AssetPackagePath);
        ObjectPath = FString::Printf(TEXT("%s.%s"), *AssetPackagePath, *AssetName);
    }

    UBehaviorTree* BTAsset = LoadObject<UBehaviorTree>(nullptr, *ObjectPath);
    if (!BTAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("BuildGraph: Could not load BehaviorTree at path: %s"), *ObjectPath);
        return;
    }

    Internal_BuildGraph(BTAsset);

    // Save the package using the new SavePackage overload (UE 5.6)
    UPackage* Package = BTAsset->GetOutermost();
    if (Package)
    {
        FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

        FSavePackageArgs SaveArgs;
        SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
        SaveArgs.SaveFlags = SAVE_None;  // you can adjust flags as needed

        bool bSaved = UPackage::SavePackage(Package, BTAsset, *Filename, SaveArgs);
        UE_LOG(LogTemp, Log, TEXT("BuildGraph: Saved package %s, success = %d"), *Filename, bSaved);

        // Open in editor after saving
        if (GEditor)
        {
            GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(BTAsset);
        }
    }
}

void FBehaviourTreeGraphModule::Internal_BuildGraph(UBehaviorTree* BTAsset)
{
    if (!BTAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("Internal_BuildGraph: BTAsset is null"));
        return;
    }

    // Create the editor graph
    UBehaviorTreeGraph* Graph = NewObject<UBehaviorTreeGraph>(BTAsset, UBehaviorTreeGraph::StaticClass(), NAME_None, RF_Transactional | RF_Public);
    Graph->SetFlags(RF_Transactional | RF_Public);

    // In UE5.6, UBehaviorTree has a member called BTGraph (UE source); assign graph
    BTAsset->BTGraph = Graph;

    // Create the root graph node
    UBehaviorTreeGraphNode_Root* RootNodeGraph = NewObject<UBehaviorTreeGraphNode_Root>(Graph, UBehaviorTreeGraphNode_Root::StaticClass(), NAME_None, RF_Transactional);
    RootNodeGraph->SetFlags(RF_Transactional | RF_Public);
    RootNodeGraph->NodeInstance = BTAsset->RootNode;
    Graph->Nodes.Add(RootNodeGraph);

    // Recursively build child nodes
    struct FBuilder
    {
        static void Build(UBehaviorTreeGraph* Graph, UBTCompositeNode* CompositeRT)
        {
            for (const FBTCompositeChild& Child : CompositeRT->Children)
            {
                if (Child.ChildComposite)
                {
                    UBehaviorTreeGraphNode_Composite* CompositeGraphNode = NewObject<UBehaviorTreeGraphNode_Composite>(
                        Graph, UBehaviorTreeGraphNode_Composite::StaticClass(), NAME_None, RF_Transactional);
                    CompositeGraphNode->NodeInstance = Child.ChildComposite;
                    Graph->Nodes.Add(CompositeGraphNode);

                    // recurse
                    Build(Graph, Child.ChildComposite);
                }
                else if (Child.ChildTask)
                {
                    UBehaviorTreeGraphNode_Task* TaskGraphNode = NewObject<UBehaviorTreeGraphNode_Task>(
                        Graph, UBehaviorTreeGraphNode_Task::StaticClass(), NAME_None, RF_Transactional);
                    TaskGraphNode->NodeInstance = Child.ChildTask.Get();
                    Graph->Nodes.Add(TaskGraphNode);
                }
            }
        }
    };

    FBuilder::Build(Graph, BTAsset->RootNode);

    // Note: This doesn't create pins / link them.
    // If you want visual wires, you'd have to:
    //   - call AllocateDefaultPins() on each UEdGraphNode (graph nodes)
    //   - then connect pins (using UEdGraphPin, plus EdGraph schema)
    //   - optionally do auto-layout
}

