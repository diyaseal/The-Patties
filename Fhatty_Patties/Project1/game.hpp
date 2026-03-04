#pragma once
void Game_Load();       
void Game_Init ();
void Game_Update();
void Game_Draw();
bool Burger_MatchesOrder(const std::vector<int>& playerBurger,
                         const std::vector<int>& customerOrder);
void Burger_Clear();
void Burger_ServeToCustomer();
void Game_Free();
void Game_Unload();

extern AEGfxTexture* Ingredients;