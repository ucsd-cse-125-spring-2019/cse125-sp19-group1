# Game Controls  

 - Game Play  
   - Arrow keys - movement  
   - '1' Button - Change to chef  
   - '2' Button - Change to racoon  
   - '3' Button - Change to cat  
   - '4' Button - Change to dog  
   - 'D' Button - Drop item in inventory  
   - 'P' Button - reset game once chef win or animal win  
   - 'Space' Button  
     - animals: 
       - Open a box on current tile  
	     - Requires animal to hold 'Space' button for 2 seconds before box is opened  
		 - If box has an item, it is dropped on the ground  
       - Drop key at gate (must be a valid key)  
	   - Construct gate (once all keys are deposited)  
	     - Construction time is saved between interacts and tracks how long animal has held 'Space' button  
		 - Requires 30s of building to open the gate  
	   - Escape with cake at gate  
	     - Gate must be fully constructed  
		 - Animal must be holding cake in inventory  
       - Unlock jail (5 hits to free an animal, has delay between hits)  
       - Pick up item from current tile or cake from table that animal is currently facing  
	 - chef:  
       - Swing net to catch animal (has a delay before you can move again)  
	   - Place animal in jail (if currently at a jail tile)  
 - For game lobby:  
   - 'Enter' Button  - Toggle player ready status  
   - 'O' button - Start game when all players are ready  


