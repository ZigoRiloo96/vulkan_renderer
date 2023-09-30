#pragma once

const f64 FIXED_TIME = 1.0f / 60.0f;

struct Timer
{
  f64 delta;
  f64 awake;
  f64 timeScale;
};

