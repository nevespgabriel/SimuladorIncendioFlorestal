#include <stdio.h>

#include <stdlib.h>

#include <stdbool.h>

#include <math.h>





#include "dados.h"



#include <SFML/Graphics.hpp>



struct espaco {

    double tipo_vegetacao;

    double densidade_vegetacao;

    double angulo_terreno;

    int estado;

};

// Queimando - estado == 1

// Com combustivel, sem ter sido incendiado - estado == 2

// Sem combustivel - estado == 3

// Completamente queimadas - estado == 4



struct espaco espacos[150][150];

//-----------------------------------------------------------------------

// Não mexa nesta sub-rotina.

//-----------------------------------------------------------------------

void Pausar (int milliseconds) {

    struct timespec ts;

    ts.tv_sec = 0;

    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    nanosleep(&ts, NULL);

}





//-----------------------------------------------------------------------

// Altere apenas o ponto indicado nesta sub-rotina.

//-----------------------------------------------------------------------

void MostrarMatriz (sf::RenderWindow *window) {



   sf::RectangleShape rect (sf::Vector2f (4, 4));



   for (int x = 0; x < 150; x++)

      for (int y = 0; y < 150; y++) {





         //-----------------------------------------------------------------

         // Mude cor aqui

         // Escolha cor conforme conteúdo de cada célula da matriz

         // Exemplo =

         // if matriz[x][y] == fogo

         //    rect.setFillColor (255, 0, 0));

         //

         switch(espacos[x][y].estado){

	      case 1:

	          rect.setFillColor (sf::Color(255, 128, 0)); //Queimando - laranja

	          break;

	      case 2:

	          rect.setFillColor (sf::Color(153, 153, 0)); //Com combustível - amarelo escuro

	          break;

	      case 3:

	          rect.setFillColor (sf::Color(51, 102, 0)); //Sem combustível - verde escuro

	          break;

	      case 4:

	          rect.setFillColor (sf::Color(96, 96, 96)); //Completamente queimados - ciza

	          break;

	  }

         //

         // Não mude absolutamente mais nada.

         //-----------------------------------------------------------------



         rect.setPosition(sf::Vector2f(x * 5.0f, y * 5.0f));

         window->draw (rect);

      }

}



// Também não mexa na linha abaixo.

#define AtualizarTela() window.clear(); MostrarMatriz (&window); window.display();





//-----------------------------------------------------------------------

// Variáveis globais prontas (NÃO DECLARE!!!)

// Para entender, abra o arquivo "dados.h"

//

//  double GTipoVegetal     [150][150]  entre -1 e 0.4

//  double GDensidadeVegetal[150][150]  entre -1 e 0.3

//  double GAnguloTerreno   [150][150]  entre 0 e 1 (radianos = 0 a 60 graus)







//   GTipoVegetal      corresponde à variável "pveg"  no paper

//   GDensidadeVegetal corresponde à variável "pden"  no paper

//   GAnguloTerreno    corresponde à variável "theta" no paper





//-----------------------------------------------------------------------



// Declare outras variáveis globais que precisar aqui.

// Lembre que você precisa criar sua matriz de autômatos celulares;

//   isso não foi feito - apenas as matrizes de dados pveg,pden,theta estão prontas







double direcao_vento = 0.7;





//-----------------------------------------------------------------------





// Escreva suas sub-rotinas aqui.



void inicializarEspacos(){

    for (int x = 0; x < 150; x++){

        for (int y = 0; y < 150; y++) {

            espacos[x][y].tipo_vegetacao = GTipoVegetal[x][y];

            espacos[x][y].densidade_vegetacao = GDensidadeVegetal[x][y];

            espacos[x][y].angulo_terreno = GAnguloTerreno[x][y];

            if((x == 73 || x == 74 || x == 75) && (y == 73 || y == 74 || y == 75)){

                espacos[x][y].estado = 1;

            } else if (espacos[x][y].tipo_vegetacao == -1 || espacos[x][y].densidade_vegetacao == -1) { // isso ocorre porque, na equação pburn = p0(1+pveg)(1+pden)pw . ps,

            //se pveg ou pden valer -1, pburn assumirá o valor 0, resultando em uma chance nula de queimada

                espacos[x][y].estado = 3; // Sem combustível

            } else{

                espacos[x][y].estado = 2;

            }

        }

    }

};



void mudarDirecaoVento(){

    double variacao = ((rand() % 100) / 100.0) * 0.1; // de 0.0 a 0.1

    if (rand() % 2 == 0) {

        direcao_vento += variacao;

    } else {

        direcao_vento -= variacao;

    }



    if (direcao_vento < 0) direcao_vento += 2 * M_PI;

    if (direcao_vento >= 2 * M_PI) direcao_vento -= 2 * M_PI;

}



void propagarAosVizinhos(int x, int y, int estados[150][150]){

    double direcoes_rad[3][3] = {

        {

            2.356,   // Superior-esquerda

            1.571,   // Cima

            0.785,   // Superior-direita

        },

        {

            3.142,   // Esquerda

            0.000,    //Centro

            0.000,   // Direita

        },

        {

           3.927,   // Inferior-esquerda

           4.712,   // Baixo

           5.498    // Inferior-direita

        }

    };

    for(int i = x-1; i<=x+1; i++){

        for(int j = y-1; j<=y+1; j++){

            if (i >= 0 && i < 150 && j >= 0 && j < 150) {

                if((i != x || j != y) && estados[i][j] == 2){

                    double direcao_propagacao = direcoes_rad[i-x+1][j-y+1];

                    double delta_theta = fabs(direcao_propagacao - direcao_vento);  // diferença angular

                    if (delta_theta > M_PI) {

                        delta_theta = 2 * M_PI - delta_theta;  // normaliza para o intervalo [0, π]

                    }

                    double pw  = exp(10 * (0.045 + 0.131 * (cos(delta_theta) - 1)));

                    double ps = exp(0.078 * espacos[i][j].angulo_terreno);

                    double pburn = 0.58 * (1 + espacos[i][j].tipo_vegetacao) * (1 + espacos[i][j].densidade_vegetacao) * pw * ps;

                    double valor_aleatorio = rand() / (double)RAND_MAX;;

                    if(valor_aleatorio < pburn){

                        estados[i][j] = 1;

                    }

                }

            }

        }

    }

}



void atualizarEstados(){

    int estados[150][150]; //guarda o estado posterior de cada um

    for (int x = 0; x < 150; x++){

        for (int y = 0; y < 150; y++) {

            estados[x][y] = espacos[x][y].estado;

        }

    }



    for (int x = 0; x < 150; x++){

        for (int y = 0; y < 150; y++) {

                if(espacos[x][y].estado == 1){

                    estados[x][y] = 4;

                    propagarAosVizinhos(x, y, estados);

                }

        }

    }

    //Os valores de estado posterior passam a ser os estados atuais

    for (int x = 0; x < 150; x++){

        for (int y = 0; y < 150; y++) {

             espacos[x][y].estado = estados[x][y];

        }

    }

}





//-----------------------------------------------------------------------





//=========================================================================



// Dentro da rotina main abaixo,

// Escreva apenas nos pontos indicados.

// Evite alterar qualquer outra coisa.





int main () {



    srand(time(NULL));

   //---------------------------------------------------------------

   // Coloque código para inicializar o simulador aqui.

   //---------------------------------------------------------------

   inicializarEspacos();



   sf::RenderWindow window(sf::VideoMode(sf::Vector2u(610, 610)), "Simulador");





   //---------------------------------------------------------------

   // Este é o laço principal de simulação.

   // São 48 horas divididas em intervalos de 20 minutos,

   //   para um total de 144 repetições.

   //---------------------------------------------------------------

   for (int tempo = 0; tempo < (3*48); tempo++) {







      //-------------------------------------------------

      // Coloque neste lugar o código para

      //   atualizar a matriz de autômatos celulares.

      // Ou seja, a cada vez que o relógio avança, é neste lugar

      //   do código que a simulação será atualizada.

      //-------------------------------------------------

      atualizarEstados();









      //-------------------------------------------------

      // A linha de código abaixo atualiza a tela.

      // Se você remover a linha a tela ficará preta,

      //   mas o simulador rodará bem mais depressa.

      //-------------------------------------------------



      if((tempo+1) % 3 == 0){

        mudarDirecaoVento();

        AtualizarTela ();

      }









      // Você pode mudar o número abaixo.

      // Isto acelera/retarda a simulação, para dar tempo de

      //   acompanhar mudanças na tela.

      Pausar (50);



   }



   return 0;

}
