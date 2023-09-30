
#include "PCH.h"

#define check_key(glfw_key, key) input->hold_keys = glfwGetKey(window->glfw_window, glfw_key) ? input->hold_keys | key : input->hold_keys

#define check_button(button, state) input->hold_buttons = state == GLFW_PRESS ? input->hold_buttons | button : input->hold_buttons

i32 active_joystick = -1;
bool joystick_update = true;

void 
ZeroJoysticks(Input* input)
{
  memset(input->axis, 0, sizeof(f32) * AXIS_COUNT);
  memset(input->button_states, 0, sizeof(u8) * BUTTON_COUNT);
}

void 
GetActiveJoystick()
{
  active_joystick = -1;

  bool present_1 = glfwJoystickPresent(GLFW_JOYSTICK_1);
  bool present_2 = glfwJoystickPresent(GLFW_JOYSTICK_2);
  bool present_3 = glfwJoystickPresent(GLFW_JOYSTICK_3);
  bool present_4 = glfwJoystickPresent(GLFW_JOYSTICK_4);

  if (present_1 || present_2 || present_3 || present_4)
  {
    int axesCount;
    const f32* axes;

    for (int joy_num = 0; joy_num < 4; joy_num++)
    {
      axes = glfwGetJoystickAxes(joy_num, &axesCount);

      if (axesCount < 4)
      {
        continue;
      }

      active_joystick = joy_num;
      break;
    }
  }
}

void 
UpdateInput(Input* input, Window* window)
{
  if (joystick_update)
  {
    joystick_update = false;
    GetActiveJoystick();

    if (active_joystick == -1)
    {
      ZeroJoysticks(input);
    }
  }

  input->hold_keys = 0x0000000000000000;
  input->hold_buttons = 0x0000000000000000;

  check_key(GLFW_KEY_W, Key::W);
  check_key(GLFW_KEY_A, Key::A);
  check_key(GLFW_KEY_S, Key::S);
  check_key(GLFW_KEY_D, Key::D);

  if (active_joystick >= 0)
  {
    input->p_axis = glfwGetJoystickAxes(active_joystick, &input->axis_count);

    input->p_button_states = glfwGetJoystickButtons(active_joystick, &input->button_count);

    if (input->p_axis)
    {
      memcpy(input->axis, input->p_axis, sizeof(f32) * AXIS_COUNT);
    }

    if (input->p_button_states)
    {
      memcpy(input->button_states, input->p_button_states, sizeof(u8) * BUTTON_COUNT);
    }

    check_button(Button::Up, input->button_states[10]);
    check_button(Button::Down, input->button_states[12]);
    check_button(Button::Right, input->button_states[11]);
    check_button(Button::Left, input->button_states[13]);
  }

  input->down_buttons = (input->hold_buttons ^ input->prev_hold_buttons) & input->hold_buttons;

  input->up_buttons = (input->hold_buttons ^ input->prev_hold_buttons) & input->prev_hold_buttons;

  input->prev_hold_buttons = input->hold_buttons;

  input->down_keys = (input->hold_keys ^ input->prev_hold_keys) & input->hold_keys;

  input->up_keys = (input->hold_keys ^ input->prev_hold_keys) & input->prev_hold_keys;

  input->prev_hold_keys = input->hold_keys;
}

bool 
GetKey(Input* input, Key key)
{
  return input->hold_keys & key;
}

bool 
GetKeyDown(Input* input, Key key)
{
  return input->down_keys & key;
}

bool 
GetKeyUp(Input* input, Key key)
{
  return input->up_keys & key;
}

bool 
GetButton(Input* input, Button button)
{
  return input->hold_buttons & button;
}

bool 
GetButtonDown(Input* input, Button button)
{
  return input->down_buttons & button;
}
bool 
GetButtonUp(Input* input, Button button)
{
  return input->up_buttons & button;
}

f32 
GetAxis(Input* input, Axis stick)
{
  return (std::abs(input->axis[stick]) < 0.15f) ? 0.0f : input->axis[stick];
}