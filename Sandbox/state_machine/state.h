#pragma once

class State {
    
public:
    virtual ~State() = default;
    virtual void load() = 0;
    virtual void update() = 0;
    virtual void draw() = 0;
};
