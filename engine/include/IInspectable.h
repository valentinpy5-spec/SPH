#pragma once

class IInspectable {
public:
    virtual ~IInspectable() = default;
    virtual void showUI() = 0; // called once per frame, inside the ImGui frame
};
