1. Load the world
2. Pause the world
3. Instance player character
4. Instance enemy actors
	For every:
	1. Instance AIController
	2. Instance BlackBoard
	3. Instance BehaviourTreeAsset
5. Loop for number of generations
	1. Select and modify behaviour trees
		1. Use heuristic to select behaviour trees that will be used this generation
		2. Mutate / crossover selected trees
		3. Fill to actor cap with best trees from last generation
	2. Reset all actors
		1. Reset position
		2. Reset stats - HP...
		3. BlackBoard
	3. Assign Behaviour Trees to enemy actors
	4. Start timer
	5. Unpause world
	6.  Wait for player died event dispatcher / timeout
	7. Pause simulation
	8. Stop all execution
		1. Behaviour trees
		2. Movement components (Motion Requests)
		3. Navmesh queries
	9. Calculate fitness
	10. Collect behaviour trees
6.  Stop simulation

---
1. Prepare environment
    1. Load the world / map / level.
    2. Capture & save a world snapshot (spawn points, navmesh state, global variables, time dilation, physics settings).
    3. Set deterministic seeds (global RNG seed and a per-run seed generator). Save seeds to run metadata.
    4. Configure simulation mode (fixed timestep, headless / no render, disable nonessential subsystems).
    5. Preload & validate assets: BehaviorTree templates, Blackboard keys, AIController classes, Tasks, Services. Fail fast on missing keys/nodes.
    6. Configure global simulation parameters (max generation count, population size, actor cap, max run time, trials per genome, logging paths).

2. Initialize & pause the world
    1. Pause the world / engine time.
    2. Start logging system and write initial run metadata (timestamp, seed, config).

3. Instance player character(s)
    1. Spawn or ensure existence of player actor(s) from templates.
    2. Initialize player state: position, rotation, stats, behavior (if player is controlled by script or human).

4. Instance enemy actor pool (do this once, reuse every generation)
    1. Spawn a fixed pool of enemy actors up to actor cap.
    2. For every actor in pool:
        1. Attach / create AIController instance.
        2. Attach / create Blackboard instance and initialize required keys with defaults.
        3. Attach / create BehaviorTree component (but do not assign runtime tree yet).
        4. Initialize actor internal state struct for fast resets (position, HP, cooldown timers, navmesh agent state).
    3. Validate pool (each actor has required components and a unique persistent ID for logging).

5. Loop for number of generations
    
    1. Prepare generation        
        1. Increment generation counter and log start time.
        2. Save generation seed(s) for reproducibility.
        
    2. Select and modify behaviour trees (produce next population)
	    
        1. Selection:
            1. Use deterministic selection method (e.g., tournament, rank; tie-break reproducible using seed).
            2. Select parent set and preserve elites (top K) directly to next gen.
               
        2. Variation:
            1. Apply crossover operator(s) (subtree swap, node exchange) with defined probability.
            2. Apply mutation operator(s) (subtree mutation, parameter perturbation, add/remove node) with defined probability.
            3. Enforce constraints: max depth, max nodes, valid blackboard references.
            4. Validate each child tree structurally and semantically (no missing tasks/keys). Mark invalid as failed and optionally replace with parent or random valid tree.
               
        3. Fill to actor cap:
            1. Keep elites from last generation.
            2. Add children from variation until population size == actor cap.
            3. If still under cap, add small random genomes or clones of best to maintain size.
        
    3. Reset all actors (fast reset using pooling)
        1. For every actor in pool:
            1. Reset transform to spawn point (or assigned spawn).
            2. Reset stats (HP, stamina, energy, ammo).
            3. Reset blackboard keys to defaults and clear runtime memory/state.
            4. Clear movement state: stop movement components, cancel navmesh queries, clear velocity, reset path-following state.
            5. Reset any custom component state (abilities cooldowns, perception caches).
            6. Reset AI internal timers and behavior tree memory (ensure BT component is not currently ticking).
        
    4. Assign Behaviour Trees to enemy actors
        
        1. Map each genome/tree to an actor.
        2. Assign the tree asset or runtime instance to the actor’s BT component.
            
        3. Initialize actor logging context: generation_id, genome_id, actor_id, trial_id.
            
    5. Start evaluation timer & trials
        
        1. For each genome you may run multiple trials:
            
            1. Set trial-specific seed (from generation seed list) and record it.
                
            2. Reset actors as above with the trial seed (if trial variation needed).
                
        2. Start per-run timeout watchdog (max duration per trial).
            
        3. Log trial start (time, seed, actor mapping).
            
    6. Unpause world and run simulation
        
        1. Unpause the world.
            
        2. Begin BT ticking and movement updates (in fixed timestep).
            
        3. Optionally reduce BT tick frequency for performance (but keep deterministic).
            
        4. Monitor important events: player died event dispatcher, actor died events, navmesh errors, stuck detection.
            
    7. Wait for termination conditions
        
        1. End trial when either:
            
            1. Player died (event fired) OR
                
            2. Timeout elapsed OR
                
            3. All enemies dead OR
                
            4. A global generation time limit reached OR
                
            5. Emergency stop / error condition triggered.
                
        2. If multiple trials per genome, collect and average their telemetry at end.
            
    8. Pause simulation and perform safe stop
        
        1. Pause the world.
            
        2. Stop/disable all BT execution for all actors (explicitly halt BT components).
            
        3. Stop movement components:
            
            1. Clear motion requests / input accumulators.
                
            2. Cancel navmesh queries and release path resources.
                
            3. Zero velocities and clear movement flags.
                
        4. Ensure no lingering asynchronous tasks remain (pathfinding callbacks, timers).
            
    9. Collect telemetry & calculate fitness
        
        1. Retrieve per-actor telemetry:
            
            1. survival_time, damage_dealt, damage_received, attacks_made, path_failures, time_near_player, objectives_completed, stuck_duration, error_flags.
                
        2. Compute normalized fitness components (each scaled to [0,1]).
            
        3. Combine components into scalar fitness using configured weights or use multi-objective storage.
            
        4. Store per-trial and aggregated fitness in run logs (CSV/JSON).
            
        5. Optionally compute behavioral signatures (for novelty search) and store them.
            
    10. Collect behaviour trees & bookkeeping
        
        1. Tag each genome with its fitness and metadata (tree size/depth, trial seeds, error flags).
            
        2. Save best genomes (elite checkpoint) to disk (store both genome encoding and a runtime-usable asset if desired).
            
        3. Archive interesting runs (top N, worst N, diverse representatives).
            
        4. Free or reuse temporary data structures for next generation.
            
    11. End of generation housekeeping
        
        1. Produce generation summary (best fitness, median, std, diversity metrics).
            
        2. Optionally adjust evolutionary hyperparameters (adaptive mutation rates, speciation thresholds).
            
        3. Persist checkpoints and commit run metadata (include git commit hash or code version).
            
        4. If using co-evolution or curriculum, update opponent pools/environment difficulty.
            
6. Stop simulation (end of all generations)
    
    1. If generation count reached or stopping condition met, finalize run.
        
    2. Pause world and perform final safe stop (halt BTs, movement, pathfinding).
        
    3. Save final population, best trees, and full run logs (per-generation and per-trial).
        
    4. Save experiment config and seeds so results are reproducible.
        
    5. Produce summary outputs:
        
        1. CSV/JSON of run metrics.
            
        2. Optional visual artifacts: plots for fitness over generations, best tree visualizations, short recordings of exemplar trials.
            
    6. Clean up:
        
        1. Release actor pool and any engine resources if required.
            
        2. Flush logs and close files.
            
        3. Optionally snapshot the run directory for archiving.
            
