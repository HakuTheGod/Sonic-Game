#pragma once
#include "App.h"

class SonicApp : public app::App {
    private:
    public:
        void Initialise (void) override;
        void Load (void) override;
        void Clear (void) override;
        void Run (void) override;
        void RunIteration (void) override;
};