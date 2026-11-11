An alternative to Balloon Buster. 
In this lab, you will be creating a duck hunter game that is a natural extension to the balloon buster
examples shown in lecture. There is a video on Brightspace that shows you what the game should look like. It
might be helpful to start from the balloon buster code from lecture and modify it to get the intended
functionality. The functionality you need to implement is as follows:
• A crossbow that can fire one bolt at a time. This is much like the crossbow in Balloon Buster but it
doesn't have to move side to side. Like Balloon Buster the crossbow should shoot when you press
space, for a total 5 times. When the 5th shot is done, the game should end.
• A never ending stream of ducks across the top with spaces between them. In order to do this you
should implement the following logic:
o You should have a PhysicsShapeList for the ducks
o You should have a timer in your main loop that counts down milliseconds between duck
creation and then creates a new duck with the PhysicsShapeList that starts off screen to the left
and has a positive x velocity. You can used the Balloon Creation code from Balloon Buster and
modify it for your needs.
o You should have a right side boundary and when a duck collides with it, it should be removed
from the list
o When an arrow collides with a duck the duck and arrow should be removed and the score
increased (again see Balloon Buster for example code.)
• You should track down your own duck images to use for the ducks flying across the screen. A png with
a transparent background should suffice.
• You should display the score in the bottom right corner and the number of arrows left in the bottom
left corner like Balloon Buster.
• When the game ends it should display GAME OVER along with the last frame until you press space
again.
