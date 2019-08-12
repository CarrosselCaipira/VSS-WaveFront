/*
 * This file is part of the VSS-SampleStrategy project.
 *
 * This Source Code Form is subject to the terms of the GNU GENERAL PUBLIC LICENSE,
 * v. 3.0. If a copy of the GPL was not distributed with this
 * file, You can obtain one at http://www.gnu.org/licenses/gpl-3.0/.
 */

#include <Communications/StateReceiver.h>
#include <Communications/CommandSender.h>
#include <Communications/DebugSender.h>
#include <cmath>
#include "cstdlib"

using namespace vss;

IStateReceiver *stateReceiver;
ICommandSender *commandSender;
IDebugSender *debugSender;

State state;

std::vector<Point> caminho;

int campo[170][130];
#define MAX 9999

void send_commands();
void send_debug();

std::ostream& operator<<(std::ostream& os, const std::vector<Point> &input) {
	for (auto const& i: input) {
		os << "[" << i.x << ", " << i.y << "] -> ";
	}
	return os;
}

float calculaDistEuclidianaRoboObj (int x, int y) {
    return std::sqrt((x - state.ball.x) * (x - state.ball.x) + (y - state.ball.y) * (y - state.ball.y));
    
}

void inicializaWaveFront(){
    for(int i = 0; i < 170; i++){
        for(int j = 0; j < 130; j++){
            if ((i < 10 && j < 30) || (i < 10 && j > 100) || (i > 160 && j < 30) || (i > 160 && j > 100)) {
                campo[i][j] = -1;
            }
            else {
                campo[i][j] = MAX;
            }
        }
    }
}

int vizinhoMinimo(int x, int y){
    int minimo = MAX;
    //          O O O
    // Calcula  O R O
    //          O O O
    if(x > 0 && x < 169){
        //          X X X
        // Calcula  O R O
        //          X X X
        if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && campo[x+1][y] != -2)
            minimo = campo[x+1][y];
        
        if (campo[x-1][y] < minimo && campo[x-1][y] != -1 && campo[x-1][y] != -2)
            minimo = campo[x-1][y];

        //          O O O
        // Calcula  X R X
        //          O O O
        if(y > 0 && y < 129){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
        //          X X X
        // Calcula  X R X
        //          O O O
        else if(y == 0){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
        }
        //          O O O
        // Calcula  X R X
        //          X X X
        else if(y == 129) {
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
    }
    //          X O O
    // Calcula  X R O
    //          X O O
    else if(x == 0){
        if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && campo[x+1][y] != -2)
            minimo = campo[x+1][y];
        
        if(y > 0 && y < 129){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
        }
        else if(y == 0){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && campo[x+1][y+1] != -2)
                minimo = campo[x+1][y+1];
        }
        else if(y == 129) {
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && campo[x+1][y-1] != -2)
                minimo = campo[x+1][y-1];
        }
    }

    //          O O X
    // Calcula  O R X
    //          O O X
    else if (x == 169) {
        if (campo[x-1][y] < minimo && campo[x+1][y] != -1 && campo[x-1][y] != -2)
            minimo = campo[x-1][y];

        if(y > 0 && y < 129){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
        else if(y == 0){
            if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && campo[x][y+1] != -2)
                minimo = campo[x][y+1];
            if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && campo[x-1][y+1] != -2)
                minimo = campo[x-1][y+1];
        }
        else if(y == 129) {
            if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && campo[x][y-1] != -2)
                minimo = campo[x][y-1];
            if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && campo[x-1][y-1] != -2)
                minimo = campo[x-1][y-1];
        }
    }
    
        
    return minimo;
}

void calculaWaveFront(){
    inicializaWaveFront();
    for(int robo = 0; robo < 3; robo++){
        for(int i = state.teamBlue[robo].x - 1; i < state.teamBlue[robo].x + 1; i++){
            for(int j = state.teamBlue[robo].y - 1; j < state.teamBlue[robo].y + 1; j++){
                campo[i][j] = -1;
            }
        }
        campo[(int) state.teamYellow[robo].x][(int) state.teamYellow[robo].y] = -1;
    }

    campo[(int) state.ball.x][(int) state.ball.y] = 0;

    bool convergiu = false;
    int valorMinimo = 0;
    int x = 0;
    int y = 0;
    while (campo[160][129] == MAX || campo[10][0] == MAX){
        x=0;
        y=0;
        convergiu = false;
        do {
            if(campo[x][y] != -1 && campo[x][y] != -2){

                valorMinimo = vizinhoMinimo(x, y);
                if(valorMinimo < MAX && x == (int)state.teamYellow[0].x && y == (int)state.teamYellow[0].y){
                    // std::cout << "Terminou o WaveFront1" << '\n';
                    convergiu = true;
                }

                else
                    if (valorMinimo != MAX && campo[x][y] > valorMinimo){
                    campo[x][y] = valorMinimo + 1;
                    // std::cout << x <<'\n';
                }
            }

            x++;
            if(x == 169 && y != 129){
                y++;
                x = 0;
            }
            else if (x == 169 && y == 129){
                convergiu = true;
                // std::cout << "Terminou o WaveFront2" << '\n';
            }
        } while (!convergiu);
    }
}

void calculaCaminho() {
    int x, y, minimo, distMin;
    bool convergiu = false;
    Point aux, auxPassado;

    x = state.teamYellow[0].x;
    y = state.teamYellow[0].y;
    minimo = MAX;

    caminho.clear();

    int contador = 0;
    do {
        contador++;
        minimo = MAX;
        distMin = MAX;
        
        auxPassado = aux;
        // std::cout << x << " " << y << " " << campo[x+1][y-1] << '\n';

        //          O O O
        // Calcula  O R O
        //          O O O
        if(x > 0 && x < 169){
            //          X X X
            // Calcula  O R O
            //          X X X
            if (campo[x-1][y] < minimo && campo[x-1][y] != -1 && calculaDistEuclidianaRoboObj(x-1, y) < distMin){
                minimo = campo[x-1][y];
                aux.x = x-1;
                aux.y = y;
                distMin = calculaDistEuclidianaRoboObj(x-1, y);
            }

            if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && calculaDistEuclidianaRoboObj(x+1, y) < distMin){
                minimo = campo[x+1][y];
                aux.x = x+1;
                aux.y = y;
                distMin = calculaDistEuclidianaRoboObj(x+1, y);
            }

            //          O O O
            // Calcula  X R X
            //          O O O
            if(y > 0 && y < 129){
                if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
                    minimo = campo[x][y+1];
                    aux.x = x;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x, y+1);
                }
                if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
                    minimo = campo[x][y-1];
                    aux.x = x;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x, y-1);
                }
                if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
                    minimo = campo[x+1][y+1];
                    aux.x = x+1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
                }
                if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
                    minimo = campo[x+1][y-1];
                    aux.x = x+1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
                }
                if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
                    minimo = campo[x-1][y+1];
                    aux.x = x-1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
                }
                if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
                    minimo = campo[x-1][y-1];
                    aux.x = x-1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
                }
            }
            //          X X X
            // Calcula  X R X
            //          O O O
            else if(y == 0){
                if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
                    minimo = campo[x][y+1];
                    aux.x = x;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x, y+1);
                }
                if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
                    minimo = campo[x+1][y+1];
                    aux.x = x+1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
                }
                if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
                    minimo = campo[x-1][y+1];
                    aux.x = x-1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
                }
            }
            //          O O O
            // Calcula  X R X
            //          X X X
            else if(y == 129) {
                if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
                    minimo = campo[x][y-1];
                    aux.x = x;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x, y-1);
                }
                if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
                    minimo = campo[x+1][y-1];
                    aux.x = x+1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
                }
                if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
                    minimo = campo[x-1][y-1];
                    aux.x = x-1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
                }
            }
        }

        //          X O O
        // Calcula  X R O
        //          X O O
        else if(x == 0){
            if (campo[x+1][y] < minimo && campo[x+1][y] != -1 && calculaDistEuclidianaRoboObj(x+1, y) < distMin){
                minimo = campo[x+1][y];
                aux.x = x+1;
                aux.y = y;
                distMin = calculaDistEuclidianaRoboObj(x+1, y);
            }
            
            if(y > 0 && y < 129){
                if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
                    minimo = campo[x][y+1];
                    aux.x = x;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x, y+1);
                }
                if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
                    minimo = campo[x][y-1];
                    aux.x = x;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x, y-1);
                }
                if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
                    minimo = campo[x+1][y+1];
                    aux.x = x+1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
                }
                if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
                    minimo = campo[x+1][y-1];
                    aux.x = x+1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
                }
            }
            else if(y == 0){
                if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
                    minimo = campo[x][y+1];
                    aux.x = x;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x, y+1);
                }
                if (campo[x+1][y+1] < minimo && campo[x+1][y+1] != -1 && calculaDistEuclidianaRoboObj(x+1, y+1) < distMin){
                    minimo = campo[x+1][y+1];
                    aux.x = x+1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y+1);
                }
            }
            else if(y == 129) {
                if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
                    minimo = campo[x][y-1];
                    aux.x = x;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x, y-1);
                }
                if (campo[x+1][y-1] < minimo && campo[x+1][y-1] != -1 && calculaDistEuclidianaRoboObj(x+1, y-1) < distMin){
                    minimo = campo[x+1][y-1];
                    aux.x = x+1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x+1, y-1);
                }
            }
        }

        //          O O X
        // Calcula  O R X
        //          O O X
        else if (x == 169) {
            if (campo[x-1][y] < minimo && campo[x+1][y] != -1 && calculaDistEuclidianaRoboObj(x-1, y) < distMin){
                minimo = campo[x-1][y];
                aux.x = x-1;
                aux.y = y;
                distMin = calculaDistEuclidianaRoboObj(x-1, y);
            }

            if(y > 0 && y < 129){
                if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
                    minimo = campo[x][y+1];
                    aux.x = x;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x, y+1);
                }
                if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
                    minimo = campo[x][y-1];
                    aux.x = x;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x, y-1);
                }
                if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
                    minimo = campo[x-1][y+1];
                    aux.x = x-1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
                }
                if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
                    minimo = campo[x-1][y-1];
                    aux.x = x-1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
                }
            }
            else if(y == 0){
                if (campo[x][y+1] < minimo && campo[x][y+1] != -1 && calculaDistEuclidianaRoboObj(x, y+1) < distMin){
                    minimo = campo[x][y+1];
                    aux.x = x;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x, y+1);
                }
                if (campo[x-1][y+1] < minimo && campo[x-1][y+1] != -1 && calculaDistEuclidianaRoboObj(x-1, y+1) < distMin){
                    minimo = campo[x-1][y+1];
                    aux.x = x-1;
                    aux.y = y+1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y+1);
                }
            }
            else if(y == 129) {
                if (campo[x][y-1] < minimo && campo[x][y-1] != -1 && calculaDistEuclidianaRoboObj(x, y-1) < distMin){
                    minimo = campo[x][y-1];
                    aux.x = x;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x, y-1);
                }
                if (campo[x-1][y-1] < minimo && campo[x-1][y-1] != -1 && calculaDistEuclidianaRoboObj(x-1, y-1) < distMin){
                    minimo = campo[x-1][y-1];
                    aux.x = x-1;
                    aux.y = y-1;
                    distMin = calculaDistEuclidianaRoboObj(x-1, y-1);
                }
            }
        }
        // std::cout << minimo << "\n";

        if (auxPassado != aux){
            caminho.push_back(aux);
            x = aux.x;
            y = aux.y;
        }
        
        if (minimo == 0){
            convergiu = true;
        }
        if(contador == 500)
            break;
        
    } while (!convergiu && contador < 200);
}

int main(int argc, char** argv){
    srand(time(NULL));

    stateReceiver = new StateReceiver();
    commandSender = new CommandSender();
    debugSender = new DebugSender();

    stateReceiver->createSocket();
    commandSender->createSocket(TeamType::Yellow);
    debugSender->createSocket(TeamType::Yellow);

    while(true){
        state = stateReceiver->receiveState(FieldTransformationType::None);
        std::cout << state << std::endl;

        // send_commands();
        send_debug();
    }

    return 0;
}

void send_commands(){
    Command command;

    for(int i = 0 ; i < 3 ; i++){
        command.commands.push_back(WheelsCommand(-35, 27));
    }

    commandSender->sendCommand(command);
}


void send_debug(){
    // vss::Debug debug;

    // for(unsigned int i = 0 ; i < 3 ; i++){
    //     debug.stepPoints.push_back(Point(85 + rand()%20, 65 + rand()%20));
    // }

    // for(unsigned int i = 0 ; i < 3 ; i++){
    //     debug.finalPoses.push_back(Pose(85 + rand()%20, 65 + rand()%20, rand()%20));
    // }

    // for(unsigned int i = 0 ; i < 3 ; i++){
    //     vss::Path path;
    //     path.points.push_back(Point(85, 65));
    //     path.points.push_back(Point(85 + rand()%20, 65 + rand()%20));
    // }

    // debugSender->sendDebug(debug);

    calculaWaveFront();
    std::cout << "CALCULA CAMINHO" << '\n';
    calculaCaminho();
    std::cout << "CALCULOU CAMINHO" << '\n';
    std::cout << caminho << "\n\n";

    // for (int x = (int) state.ball.x - 30; x < state.ball.x + 30; x++){
    //     for(int y = (int) state.ball.y - 30; y < state.ball.y + 30; y++){
    //         if (campo[x][y] != 9999)
    //             std::cout << campo[x][y] << " ";
    //     }
    //     std::cout << '\n';
    // }

    // for (int x = (int) state.ball.x - 30; x < state.teamYellow[0].x; x++){
    //         if (campo[x][(int) state.ball.y] != 9999)
    //             std::cout << campo[x][(int) state.ball.y] << " ";
    // }
}
