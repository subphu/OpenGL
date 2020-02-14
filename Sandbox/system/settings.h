//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#include "define.h"

class Settings {
    
public:
    const char* name = "Sandbox";
    float frameTime = 1.f/60.f;
    
    Settings() { }
    ~Settings() { }
    
    Size<int> windowSize() { return _windowSize; }
    Size<int> bufferSize() { return _bufferSize; }
    float ratio() { return _ratio; }
    
    void setWindowSize(Size<int> size) {
        _windowSize = size;
        calcRatio(size);
    }
    
    void setBufferSize(Size<int> size) {
        _bufferSize = size;
        calcRatio(size);
    }
    
private:
    Size<int> _windowSize = { 900, 900 };
    Size<int> _bufferSize = { 900, 900 };
    float _ratio = 1.f;
    
    void calcRatio(Size<int> size) { _ratio = (float)size.width / (float)size.height; }
    
};
