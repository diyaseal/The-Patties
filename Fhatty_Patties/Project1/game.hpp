// ---------------------------------------------------------------------------
// Game.hpp - Header file for the core gameplay state.
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#pragma once
void Game_Load();       
void Game_Init ();
void Game_Update();
void Game_Draw();
bool Burger_MatchesOrder(const std::vector<int>& playerBurger,
                         const std::vector<int>& customerOrder);
void Burger_Clear();
void Game_Free();
void Game_Unload();

extern AEGfxTexture* Ingredients;
extern AEGfxTexture* GameplayBG_back;
extern AEGfxTexture* GameplayBG_front;
extern AEGfxTexture* CustomerTexture[4];
extern AEGfxTexture* PlateTexture;
extern AEGfxTexture* LockTexture;
extern AEGfxTexture* SodaTexture;

