#pragma once
#include <functional>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdint>

namespace app
{

    class Game
    {
    public:
        using Action = std::function<void(void)>;
        using Pred = std::function<bool(void)>;

    private:
        Action render, anim, input, ai, physics, destruct, collisions, user;
        Pred done;

        Action pauseResume;
        bool isPaused = false;
        uint64_t pauseTime = 0;

        void Invoke(const Action &f)
        {
            if (f)
                f();
        }

    public:
        void SetRender(const Action &f)
        {
            render = f;
        }
        // rest of setters are similary defined

        void Render(void) { Invoke(render); }
        void ProgressAnimations(void) { Invoke(anim); }
        void Input(void) { Invoke(input); }
        void AI(void) { Invoke(ai); }
        void Physics(void) { Invoke(physics); }
        void CollisionChecking(void) { Invoke(collisions); }
        void CommitDestructions(void) { Invoke(destruct); }
        void UserCode(void) { Invoke(user); }
        bool IsFinished(void) const { return !done(); }

        void SetOnPauseResume(const Action &f) { pauseResume = f; }
        void Pause(uint64_t t)
        {
            isPaused = true;
            pauseTime = t;
            Invoke(pauseResume);
        }
        void Resume(void)
        {
            isPaused = false;
            Invoke(pauseResume);
            pauseTime = 0;
        }
        bool IsPaused(void) const { return isPaused; }
        uint64_t GetPauseTime(void) const { return pauseTime; }

        void MainLoop(void);
        void MainLoopIteration(void);
    };

    class App
    {
    protected:
        Game game;

    public:
        virtual void Initialise(void) = 0;
        virtual void Load(void) = 0;
        virtual void Run(void) { game.MainLoop(); }
        virtual void RunIteration(void)
        {
            game.MainLoopIteration();
        }
        Game &GetGame(void) { return game; }
        const Game &GetGame(void) const { return game; }
        virtual void Clear(void) = 0;
        void Main(void)
        {
            Initialise();
            Load();
            Run();
            Clear();
        }
    };
}