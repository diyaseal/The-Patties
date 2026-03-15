#pragma once
void Points_Load();
void Points_Init();
void Points_Update();
void Points_Draw();
void Points_Free();
void Points_Unload();

// Call this when a customer is served to add points
void Points_AddPoints(bool cust);

extern float currentPoints;  // declaration — no value
extern int currentStars;