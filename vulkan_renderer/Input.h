#pragma once

typedef enum
{
  LEFT_STICK_X  = 0,
  LEFT_STICK_Y  = 1,
  RIGHT_STICK_X = 2,
  RIGHT_STICK_Y = 3,
  LEFT_TRIGGER  = 4,
  RIGHT_TRIGGER = 5,
  AXIS_COUNT = 6
} Axis;

typedef enum
{
  W = BIT(0),
  S = BIT(1),
  A = BIT(2),
  D = BIT(3),
  //COUNT = 4
} Key;

typedef enum
{
  Down = BIT(0),
  Up = BIT(1),
  Right = BIT(2),
  Left = BIT(3),
  BUTTON_COUNT = 16
} Button;

struct Input
{
  i32 hold_keys;
  i32 prev_hold_keys;
  i32 down_keys;
  i32 up_keys;

  i32 hold_buttons;
  i32 prev_hold_buttons;
  i32 down_buttons;
  i32 up_buttons;

  i32 axis_count;
  i32 button_count;

  static i32 active_joystick;

  f32 axis[AXIS_COUNT]
  {
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      0.0f
  };

  u8 button_states[BUTTON_COUNT]
  {
      0
  };

  const f32* p_axis;
  const u8* p_button_states;
};

void
UpdateInput(Input* input, Window* window);

bool
GetKey(Input* input, Key key);

bool
GetKeyDown(Input* input, Key key);

bool
GetKeyUp(Input* input, Key key);

bool
GetButton(Input* input, Button button);

bool
GetButtonDown(Input* input, Button button);

bool
GetButtonUp(Input* input, Button button);

f32
GetAxis(Input* input, Axis stick);