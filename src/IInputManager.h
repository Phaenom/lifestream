#ifndef I_INPUT_MANAGER_H
#define I_INPUT_MANAGER_H

// Interface for input handling in the application.
// Provides methods to initialize input systems, update input states,
// and query input events such as rotation and button presses.
class IInputManager {
public:
    // Initialize the input manager and prepare it for use.
    virtual void begin() = 0;

    // Update the input states, should be called regularly (e.g., each frame).
    virtual void update() = 0;

    // Get the current rotation input value.
    virtual long update_encoder();

    // Check if the input button was short pressed.
    //virtual bool get_encoder_button();

    // Check if the input button was long pressed.
    //virtual bool get_arcade_button();

    virtual bool isEncoderButtonReleased() ;

    virtual bool isArcadeButtonReleased() ;

    virtual ~IInputManager() {}
};

#endif