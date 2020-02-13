#pragma once

class State {
    
public:
    virtual ~State() = default;
    virtual void load() = 0;
    virtual long run(long lag) = 0;
};
