#include "BehaviourTreeGraphModule.h"

#include "BehaviorTreeGraph.h"
#include "BehaviorTreeGraphNode.h"
#include "BehaviorTreeGraphNode_Composite.h"
#include "BehaviorTreeGraphNode_Root.h"
#include "BehaviorTreeGraphNode_Task.h"
#include "BehaviorTreeGraphNode_Decorator.h"
#include "BehaviorTreeGraphNode_Service.h"

#include "BehaviorTree/BTCompositeNode.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/BTService.h"

#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "Kismet2/BlueprintEditorUtils.h"

#include "UObject/SavePackage.h"
#include "Misc/PackageName.h"
#include "Editor.h"
#include "Editor/UnrealEd/Public/FileHelpers.h"

IMPLEMENT_MODULE(FBehaviourTreeGraphModule, BehaviourTreeGraph)

void FBehaviourTreeGraphModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("BehaviourTreeGraph Module Started"));
}

void FBehaviourTreeGraphModule::ShutdownModule()
{
}

void FBehaviourTreeGraphModule::BuildGraphForBehaviorTree(const FString& AssetPackagePath)
{
	if (!GIsEditor) return;

	// 1. Resolve Path
	FString ObjectPath = AssetPackagePath;
	if (!AssetPackagePath.Contains(TEXT(".")))
	{
		FString AssetName = FPackageName::GetShortName(AssetPackagePath);
		ObjectPath = FString::Printf(TEXT("%s.%s"), *AssetPackagePath, *AssetName);
	}

	// 2. Load Asset
	UBehaviorTree* BTAsset = LoadObject<UBehaviorTree>(nullptr, *ObjectPath);
	if (!BTAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("BuildGraph: Could not load BehaviorTree at path: %s"), *ObjectPath);
		return;
	}

	// 3. Rebuild Graph
	Internal_BuildGraph(BTAsset);

	// 4. Save
	UPackage* Package = BTAsset->GetOutermost();
	if (Package)
	{
		FString Filename = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
		
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		
		bool bSaved = UPackage::SavePackage(Package, BTAsset, *Filename, SaveArgs);
		UE_LOG(LogTemp, Log, TEXT("BuildGraph: Rebuilt and Saved package %s (Success: %d)"), *Filename, bSaved);
	}
}

void FBehaviourTreeGraphModule::Internal_BuildGraph(UBehaviorTree* BTAsset)
{
	if (!BTAsset) return;

	// --- 1. Clean existing Graph ---
	if (BTAsset->BTGraph)
	{
		BTAsset->BTGraph->MarkAsGarbage();
		BTAsset->BTGraph = nullptr;
	}

	// --- 2. Create New Graph ---
	UBehaviorTreeGraph* Graph = NewObject<UBehaviorTreeGraph>(BTAsset, UBehaviorTreeGraph::StaticClass(), NAME_None, RF_Transactional | RF_Public);
	BTAsset->BTGraph = Graph;
	
	// Set Schema (Essential for Pin Connections)
	const UEdGraphSchema* Schema = Graph->GetSchema();
	if (!Schema)
	{
		// Fallback if schema isn't automatically set
		// Graph->Schema = UBehaviorTreeGraphSchema::StaticClass(); 
	}

	Graph->GetSchema()->CreateDefaultNodesForGraph(*Graph);

	// The default create might create a RootNode, let's find it or create one.
	UBehaviorTreeGraphNode_Root* RootGraphNode = nullptr;

	for (UEdGraphNode* Node : Graph->Nodes)
	{
		if (auto R = Cast<UBehaviorTreeGraphNode_Root>(Node))
		{
			RootGraphNode = R;
			break;
		}
	}

	if (!RootGraphNode)
	{
		RootGraphNode = NewObject<UBehaviorTreeGraphNode_Root>(Graph);
		RootGraphNode->CreateNewGuid();
		Graph->AddNode(RootGraphNode);
	}

	// Link Root Graph Node to Runtime Root
	RootGraphNode->NodeInstance = BTAsset->RootNode;
	RootGraphNode->AllocateDefaultPins(); // CRITICAL: Creates the "Output" pin on Root

	// --- 3. Recursive Rebuild ---
	if (BTAsset->RootNode)
	{
		int32 XTracker = 0;
		// Recursively build children
		UBehaviorTreeGraphNode* ChildGraphNode = CreateGraphNodeForRuntimeNode(Graph, BTAsset->RootNode, 1, XTracker);
		
		if (ChildGraphNode)
		{
			CreateConnection(RootGraphNode, ChildGraphNode);
		}
	}

	// --- 4. Notify Editor ---
	Graph->NotifyGraphChanged();
	BTAsset->PostEditChange();
	BTAsset->MarkPackageDirty();
}

UBehaviorTreeGraphNode* FBehaviourTreeGraphModule::CreateGraphNodeForRuntimeNode(UBehaviorTreeGraph* Graph, UBTNode* RuntimeNode, int32 Depth, int32& CurrentXCounter)
{
	if (!RuntimeNode) return nullptr;

	UBehaviorTreeGraphNode* NewGraphNode = nullptr;

	// 1. Determine Class Type
	if (RuntimeNode->IsA(UBTCompositeNode::StaticClass()))
	{
		NewGraphNode = NewObject<UBehaviorTreeGraphNode_Composite>(Graph);
	}
	else if (RuntimeNode->IsA(UBTTaskNode::StaticClass()))
	{
		NewGraphNode = NewObject<UBehaviorTreeGraphNode_Task>(Graph);
	}
	else
	{
		// Fallback
		return nullptr;
	}

	// 2. Setup Basic Node
	NewGraphNode->CreateNewGuid();
	NewGraphNode->NodeInstance = RuntimeNode;
	Graph->AddNode(NewGraphNode);

	// 3. Layout (Simple Grid)
	const int32 X_SPACING = 250;
	const int32 Y_SPACING = 150;
	
	NewGraphNode->NodePosX = CurrentXCounter * X_SPACING;
	NewGraphNode->NodePosY = Depth * Y_SPACING;
	
	CurrentXCounter++; // Increment X so next sibling is to the right

	// 4. Create Pins (CRITICAL)
	NewGraphNode->AllocateDefaultPins();

	// 5. Handle Decorators & Services (if Composite)
	if (UBTCompositeNode* CompRuntime = Cast<UBTCompositeNode>(RuntimeNode))
	{
		RebuildSubNodes(NewGraphNode, CompRuntime);

		// 6. Recurse Children
		for (FBTCompositeChild& Child : CompRuntime->Children)
		{
			UBTNode* ChildRuntimeNode = Child.ChildComposite ? (UBTNode*)Child.ChildComposite : (UBTNode*)Child.ChildTask;
			
			if (ChildRuntimeNode)
			{
				UBehaviorTreeGraphNode* ChildGraphNode = CreateGraphNodeForRuntimeNode(Graph, ChildRuntimeNode, Depth + 1, CurrentXCounter);
				if (ChildGraphNode)
				{
					// Re-attach decorators specific to this child link?
					// Note: UE BT stores Child-specific decorators in the Composite Child Link, 
					// but visually they often attach to the child node or the link. 
					// *However*, in the GraphNode structure, Decorators usually sit on the parent composite's list 
					// OR on the child node itself depending on implementation. 
					// The simplest reconstruction is usually attaching logic to nodes.
					
					CreateConnection(NewGraphNode, ChildGraphNode);
				}
			}
		}
	}

	return NewGraphNode;
}

void FBehaviourTreeGraphModule::RebuildSubNodes(UBehaviorTreeGraphNode* HostGraphNode, UBTCompositeNode* RuntimeComposite)
{
	if (!HostGraphNode || !RuntimeComposite) return;

	UBehaviorTreeGraph* Graph = Cast<UBehaviorTreeGraph>(HostGraphNode->GetGraph());

	// 1. Services
	for (UBTService* Srv : RuntimeComposite->Services)
	{
		UBehaviorTreeGraphNode_Service* SrvGraphNode = NewObject<UBehaviorTreeGraphNode_Service>(HostGraphNode);
		SrvGraphNode->NodeInstance = Srv;
		HostGraphNode->Services.Add(SrvGraphNode);
		
		// Note: We do NOT add SrvGraphNode to Graph->Nodes. They are sub-objects of the HostNode.
	}

	// 2. Decorators? 
	// Decorators in UE Runtime are stored in FBTCompositeChild inside the parent.
	// But in the Editor Graph, they are attached to the Host Node (if valid).
	// This part is tricky because Runtime separates Composite-Decorators vs Child-Decorators.
	// For this simple rebuild, we only handle Composite-level logic if any, or skip if complex.
}

void FBehaviourTreeGraphModule::CreateConnection(UBehaviorTreeGraphNode* ParentNode, UBehaviorTreeGraphNode* ChildNode)
{
	if (!ParentNode || !ChildNode) return;

	// Find Output Pin on Parent
	UEdGraphPin* OutputPin = nullptr;
	for (UEdGraphPin* Pin : ParentNode->Pins)
	{
		if (Pin->Direction == EGPD_Output)
		{
			OutputPin = Pin;
			break;
		}
	}

	// Find Input Pin on Child
	UEdGraphPin* InputPin = nullptr;
	for (UEdGraphPin* Pin : ChildNode->Pins)
	{
		if (Pin->Direction == EGPD_Input)
		{
			InputPin = Pin;
			break;
		}
	}

	// Link
	if (OutputPin && InputPin)
	{
		OutputPin->MakeLinkTo(InputPin);
	}
}