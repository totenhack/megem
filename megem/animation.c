int reaction_time_running = 0, reaction_time_milliseconds = 0;

float easeInOutQuart(float t, float b, float c, float d) {
  t /= d/2;
	if (t < 1) return c/2*t*t + b;
	t--;
	return -c/2 * (t*(t-2) - 1) + b;
}

void reactionTime_loop() {
  reaction_time_running = 1;

  DWORD initial = timeGetTime();
  reaction_time_milliseconds = 0;
  while (reaction_time_milliseconds < 1500) {
    setGameValue(D_GAME_SPEED, floatToInt(
        easeInOutQuart(reaction_time_milliseconds, 1, -0.75, 1500)));

    reaction_time_milliseconds = timeGetTime() - initial;
    Sleep(1);
  }

  initial = timeGetTime();
  reaction_time_milliseconds = 0;
  while (reaction_time_milliseconds < 7.5 * 1000) {
    setGameValue(D_GAME_SPEED, floatToInt(0.25));
    reaction_time_milliseconds = timeGetTime() - initial;
    Sleep(1);
  }

  initial = timeGetTime();
  reaction_time_milliseconds = 0;
  while (reaction_time_milliseconds < 1500) {
    setGameValue(D_GAME_SPEED, floatToInt(
        easeInOutQuart(reaction_time_milliseconds, 0.25, 0.75, 1500)));

    reaction_time_milliseconds = timeGetTime() - initial;
    Sleep(1);
  }

  setGameValue(D_GAME_SPEED, floatToInt(1));
  reaction_time_running = 0;
}

void reactionTime() {
  if (reaction_time_running) {
    reaction_time_milliseconds = 1500;
    Sleep(17);
    reaction_time_milliseconds = 0;
    return;
  }

  CreateThread(0, 0, (LPTHREAD_START_ROUTINE)reactionTime_loop, 0, 0, 0);
}
