#include <iostream>
#include <ctime> // Borrowing rand() & srand() from here
#include <cmath> // Borrowing abs() from here

// #include <conio.h> // Borrowing getch() but I was told we are not allowed to use this library

using namespace std;

/* 
    Generates either a vertical 3 block wall or a horizontal 3 block wall and places it on the feild
    But before it places it on the feild, it makes sure that there is a ring of pellets around it so there
    is no possibility of us accidentally trapping the ghost/player/pellet inside 4 walls.
    Also we reduce total pellets by 3 each time a wall is palced.
*/

void wallGenerator(char map[15][41], int horizontalOffSet, int verticalOffSet, int &total) {
        int randomX, randomY;
        bool doWallsExist;
        do {
            doWallsExist = false;
            randomX = (rand() % 9) + 3;
            randomY = (rand() % 35) + 3;

            for(int i = randomX - 2; i <= randomX + 2; i++) {
                for(int j = randomY - 2; j <= randomY + 2; j++) {
                    if (map[i][j] == '#') {
                        doWallsExist = true;
                    }
                }   
            }
        } while (doWallsExist);
        total -= 3;
        map[randomX][randomY] = '#';
        map[randomX + horizontalOffSet][randomY + verticalOffSet] = '#';
        map[randomX - horizontalOffSet][randomY - verticalOffSet] = '#';
}

/*
    Generates a 15 x 41 map (I tried to keep it odd so I can place our Pacman in the middle)
    First it initializes the map with empty spaces (to avoid #'s from a prior map from entering the feild)
    Then if fills out the empty spaces with pellets
    Then it places 10 Horizontal and 10 Vertical walls randomly in the map
*/

void genMap (char map[15][41], int &total, char &pellet) {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 41; j++) {
            map[i][j] = ' '; 
        }
    }

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 41; j++) {
            map[0][j] = '#';
            map[14][j] = '#';
            map[i][0] = '#';
            map[i][40] = '#';
            if (!(map[i][j] == '#')) {
                map[i][j] = pellet;
                total++;
            }
        }
    }

    for (int count = 0; count < 10; count++) {
        wallGenerator(map, 1, 0, total);
        wallGenerator(map, 0, 1, total);
    }
}

/*
    Takes in proper input from user and changes the position of our pacman according to the controls we assigned
    If there is a wall in the way, the pacman will not move but a turn will be used up to penalize
    Each time pacman eats a pellet, the consumed counter will go up (called by reference)
*/

void playerMovement(char &playerDirection, char map[15][41], char &playerX, char &playerY, int &consumed, char &pellet) {
    
    // playerDirection = getch();
    bool input;

    do {
        cin >> playerDirection;
        input = playerDirection == 'w' || playerDirection == 'a' || playerDirection == 's' || playerDirection == 'd';
        if (!input) {
            cout << "Invalid input. Try again: ";
        }
    } while (!input);
    
    switch (playerDirection) {
        case 'w':
            if (map[playerX - 1][playerY] != '#') {
                if (map[playerX - 1][playerY] == pellet) {
                    consumed++;
                }
                map[playerX--][playerY] = ' ';
            }
            map[playerX][playerY] = '^';
            break;
        case 'a':
            if (map[playerX][playerY - 1] != '#') {
                if (map[playerX][playerY - 1] == pellet) {
                    consumed++;
                }
                map[playerX][playerY--] = ' ';
            }
            map[playerX][playerY] = '<';
            break;
        case 's':
            if (map[playerX + 1][playerY] != '#') {
                if (map[playerX + 1][playerY] == pellet) {
                    consumed++;
                }
                map[playerX++][playerY] = ' ';
            }
            map[playerX][playerY] = 'v';
            break;
        case 'd':
            if (map[playerX][playerY + 1] != '#') {
                if (map[playerX][playerY + 1] == pellet) {
                    consumed++;
                }
                map[playerX][playerY++] = ' ';
            }
            map[playerX][playerY] = '>';
            break;             
    }

}    

/*
    Picks a random direction using rand() function and moves there by 1 block, unless there is a wall or another
    ghost in the way. The ghosts do not delete the pellets, they basically 'shift' whatever is infront of them 
    to their location as they move.
*/

void ghostMovement(char map[15][41], char &ghostX, char &ghostY) {
    int ghostDirection = rand() % 4;
    switch (ghostDirection) {
        case 0:
            if (map[ghostX - 1][ghostY] != '#' && map[ghostX - 1][ghostY] != '8')
            map[ghostX--][ghostY] = map[ghostX - 1][ghostY];
            map[ghostX][ghostY] = '8';
            break;
        case 1:
            if (map[ghostX][ghostY - 1] != '#' && map[ghostX][ghostY - 1] != '8')
            map[ghostX][ghostY--] = map[ghostX][ghostY - 1];
            map[ghostX][ghostY] = '8';
            break;
        case 2:
            if (map[ghostX + 1][ghostY] != '#' && map[ghostX + 1][ghostY] != '8')
            map[ghostX++][ghostY] = map[ghostX + 1][ghostY];
            map[ghostX][ghostY] = '8';
            break;
        case 3:
            if (map[ghostX][ghostY + 1] != '#' && map[ghostX][ghostY + 1] != '8')
            map[ghostX][ghostY++] = map[ghostX][ghostY + 1];
            map[ghostX][ghostY] = '8';
            break;             
    }

}

/*
    This one is a bit hard to explain but i'll try my best

    First, I tried to make the ghost be able to follow me on the x-axis, using the ghost as origin point and 
    trying to close the horizontal distance between the ghost and pacman by moving the ghost left or right accordingly

    Next, I tried to make the ghost be able to follow me on the y-axis, using the ghost as origin point and 
    trying to close the vertical distance between the ghost and pacman by moving the ghost up or down accordingly

    Now triyng to put this all together, I first calculate the magnitude (using absolute function to avoid errors) 
    of horizontal and vertical distance and compare it with eachother to prioritize which distance to minimize.

    If Vertical distance is greater, it'll focus on shortening the vertical distance.
    Then it decides to go up or down or remain where it's at depending on the sign and value

    If Horizontal distance is greater, it'll focus on shortening the horizontal distance
    Then it decides to go left or right or remain where it's at depending on the sign and value

    If both are equal, it'll still focus on horizontal distance (Since the grid is larger horizontally than vertically)
*/

void ghostSmartMovement(char map[15][41], char &ghostX, char &ghostY, char playerX, char playerY) {
    int horizontalDistance = ghostX - playerX; // If +ve, ghost should move up, If -ve ghost should move down
    int verticalDistance = ghostY - playerY; // If +ve, ghost should move up, If -ve ghost should move down
    int direction = abs(verticalDistance) > abs(horizontalDistance) ? 1 : 0;

    switch (direction) {
        case 0:
            if (horizontalDistance > 0) { // Go Left
                if (map[ghostX - 1][ghostY] != '#' && map[ghostX - 1][ghostY] != '8') {
                    map[ghostX--][ghostY] = map[ghostX - 1][ghostY];
                }
                map[ghostX][ghostY] = '8';
            } else if (horizontalDistance < 0) { // Go Right
                if (map[ghostX + 1][ghostY] != '#' && map[ghostX + 1][ghostY] != '8') {
                    map[ghostX++][ghostY] = map[ghostX + 1][ghostY];
                }
                map[ghostX][ghostY] = '8';
            } else {
                map[ghostX][ghostY] = '8';
            }
            break;
        case 1:
            if (verticalDistance > 0) { // Go up
                if (map[ghostX][ghostY - 1] != '#' && map[ghostX][ghostY - 1] != '8') {
                    map[ghostX][ghostY--] = map[ghostX][ghostY - 1];
                }
                map[ghostX][ghostY] = '8';
            } else if (verticalDistance < 0) { // Go down
                if (map[ghostX][ghostY + 1] != '#' && map[ghostX][ghostY + 1] != '8') {
                    map[ghostX][ghostY++] = map[ghostX][ghostY + 1];
                }    
                map[ghostX][ghostY] = '8';
            } else {
                map[ghostX][ghostY] = '8';
            }
            break;
    }
} 

/*
    Clears the screen using a system command
    Then id displays the map and the controls
*/

void displayMap(char map[15][41], int consumed, int total) {
    system("clear");
    // system("cls");

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 41; j++) {
            cout << map[i][j];
        }
        cout << endl;
    }

    cout << "\n# - Wall\no - Pellet\n8 - Ghost\n> - Pacman\n";
    cout << "\nCONTROLS\n\nType W to move UP\nType A to move LEFT\nType S to move DOWN\nType D to move RIGHT\n\n";
    cout << "Pellets: " << consumed << "/" << total << "\n\n";
    cout << "Enter a key: "; 
}

/* 
    First, it generates the map with pellets and randomized walls using srand() and rand()
    Second,  it spawns in the player and ghosts and places it on the field
    Third, it displays the map and controls
    Fourth, it calls the function to move pacman
    Fifth, it calls the functions to move the randomized path ghosts and the euclidean distance pathing ghost
    Sixth, it checks if a ghost's position overlaps with a player's position. If so, then player is flagged as dead.
    Seventh, a do while loop is run on all of these steps until the player dies, or eats up all the pellets on the map.
    Eight, it displays the results if you won or lost

    Note: Please switch between system("clear") and system("cls") if you are on linux or windows respectively
*/

int main() {

    srand(time(0));
    char map[15][41], pellet = 'o';
    int total = -5, consumed = 0; // Total from -5 to compensate for the 4 ghosts + pacman that spawn and take over a pellet space
    genMap(map, total, pellet);

    char playerDirection, player = 'v', playerX = 1, playerY = 20;
    bool playerAlive = true;
    char ghost = '8';
    char ghost1X = 1, ghost1Y = 1;
    char ghost2X = 1, ghost2Y = 39;
    char ghost3X = 13, ghost3Y = 1;
    char ghostSpecialX = 13, ghostSpecialY = 39;

    map[playerX][playerY] = player;
    map[ghost1X][ghost1Y] = ghost;
    map[ghost2X][ghost2Y] = ghost;
    map[ghost3X][ghost3Y] = ghost;
    map[ghostSpecialX][ghostSpecialY] = ghost;
    
    do {

        displayMap(map, consumed, total);
        playerMovement(playerDirection, map, playerX, playerY, consumed, pellet);
        ghostSmartMovement(map, ghostSpecialX, ghostSpecialY, playerX, playerY);
        ghostMovement(map, ghost1X, ghost1Y);
        ghostMovement(map, ghost2X, ghost2Y);
        ghostMovement(map, ghost3X, ghost3Y);

        if ((playerX == ghost1X && playerY == ghost1Y) || (playerX == ghost2X && playerY == ghost2Y) || (playerX == ghost3X && playerY == ghost3Y) || (playerX == ghostSpecialX && playerY == ghostSpecialY)) {
            playerAlive = false;
        }

    } while (playerAlive && consumed != total);

    system("clear");
    // system("cls");

    if (playerAlive) {
        cout << "You won!" << endl;
    } else {
        cout << "You lost :(." << endl << "Total Pellets Collected: " << consumed << "/" << total << endl;
    }
    return 0;
}