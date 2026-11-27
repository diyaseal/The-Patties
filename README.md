Main Game:
Fatty Patty is a 2D burger-stall game my team made at DigiPen using C++ and the DigiPen Alpha Engine. You read each customer's order, stack the ingredients, and serve them before they lose patience. I worked on how players select ingredients, how orders appear, and how the game checks what was served.
Getting orders right:
A burger can have all the right ingredients and still be wrong if they're stacked out of order. Soda adds another rule: it's part of the order, but it isn't part of the stack. I split the logic into two checks so the game could validate the stack and the drink separately. I added messages for a drink mismatch or an ingredient in the wrong position, so a rejected order had a specific reason in the debug log. Order validation commit: https://github.com/diyaseal/The-Patties/commit/f4af193a1af4865134151b05d85e7dc4b175ed24 

I also wired up clicks for six ingredients and logged which item the game registered. As more ingredients unlocked, the customer bubbles needed room for longer orders. I limited requests to unlocked ingredients and wrapped the icons at four per row. Ingredient input commit · Order bubble commit
https://github.com/diyaseal/The-Patties/commit/3404ec6815fa83343887e8dfccdeaff7995445c7https://github.com/diyaseal/The-Patties/commit/b73eacc1d3a5ec0dad29f145e38fe0752f18523d

Try it out: 
For an order that includes soda, serve the right burger without the drink. Then try serving the right ingredients in the wrong order. Both should be rejected, for different reasons.

On Windows, open [Project1.sln](Fhatty_Patties/Project1.sln) in Visual Studio and select an x64 build configuration. Keep the Assets and Extern folders alongside the solution.
