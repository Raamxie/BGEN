#include "Misc/AutomationTest.h"
#include "GeneticFitnessTracker.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Engine/DamageEvents.h" 

// Changed ApplicationContextMask to EditorContext
BEGIN_DEFINE_SPEC(FGeneticFitnessIntegrationSpec, "GeneticGeneration.Integration.FitnessTracker", EAutomationTestFlags::ProductFilter | EAutomationTestFlags::EditorContext)
	UWorld* TestWorld;
	AActor* DummyEnemy;
	UGeneticFitnessTracker* Tracker;
END_DEFINE_SPEC(FGeneticFitnessIntegrationSpec)

void FGeneticFitnessIntegrationSpec::Define()
{
	BeforeEach([this]()
	{
		// 1. Create a transient test world in memory.
		TestWorld = UWorld::CreateWorld(EWorldType::Game, false);
		FWorldContext& WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.SetCurrentWorld(TestWorld);

		// 2. Spawn a dummy Actor to represent the Enemy
		FActorSpawnParameters SpawnParams;
		DummyEnemy = TestWorld->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		// 3. Create and register the Fitness Tracker Component
		Tracker = NewObject<UGeneticFitnessTracker>(DummyEnemy);
		Tracker->RegisterComponent();
		DummyEnemy->AddInstanceComponent(Tracker);

		// Initialize default config parameters as defined in your header
		Tracker->BaseFitnessScore = 500.0f;
		Tracker->DamageTakenPenalty = 1.5f;
		Tracker->PacifistPenalty = 100.0f;
		Tracker->IdlePenalty = 150.0f;
		Tracker->SmallTreePenalty = 200.0f;

		// Begin tracking (passing nullptr as Player for isolated testing)
		Tracker->BeginTracking(nullptr);
	});

	It("Should calculate base fitness with all penalties applied for doing nothing", [this]()
	{
		// Force a world tick to update time
		TestWorld->Tick(LEVELTICK_All, 1.0f);

		float FinalFitness = Tracker->CalculateFitness();

		// Calculation: 
		// Base (500) - Pacifist (100) - Idle (150) - SmallTree (200) = 50.
		// (Assuming time alive is negligible and no tree is attached)
		TestTrueExpr(FinalFitness <= 51.0f && FinalFitness >= 49.0f);
	});

	It("Should reduce fitness correctly when damage is taken", [this]()
	{
		TestWorld->Tick(LEVELTICK_All, 1.0f);

		// Use Unreal's damage system to apply 100 damage to our dummy
		FDamageEvent DamageEvent;
		DummyEnemy->TakeDamage(100.0f, DamageEvent, nullptr, nullptr);

		float FinalFitness = Tracker->CalculateFitness();

		// Calculation: 
		// 50 (from doing nothing) - (100 damage * 1.5 penalty) = -100.
		// Our function FMath::Max(1.0f, FinalFitness) clamps it to 1.0f.
		TestEqual("Fitness should bottom out at 1.0f", FinalFitness, 1.0f);
	});

	It("Should increase fitness when custom rewards are added", [this]()
	{
		TestWorld->Tick(LEVELTICK_All, 1.0f);

		Tracker->AddCustomReward(500.0f);
		float FinalFitness = Tracker->CalculateFitness();

		// Calculation: 
		// 50 (from doing nothing) + 500 (Custom Reward) = 550.
		TestTrueExpr(FinalFitness <= 551.0f && FinalFitness >= 549.0f);
	});

	AfterEach([this]()
	{
		// Clean up the dummy world so we don't leak memory between tests
		if (TestWorld)
		{
			GEngine->DestroyWorldContext(TestWorld);
			TestWorld->DestroyWorld(false);
			TestWorld = nullptr;
		}
	});
}