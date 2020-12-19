/*
Nombre: practica 3
Autores : Cristhian Rodriguez Gomez y Luis Mathioux Abad
Fecha : 21 / 02 / 2017
Fecha límite de entrega : 26 de marzo de 2017
Descripcion : juego Sokoan
*/
#include <iostream>
#include <locale.h>
#include <Windows.h>
#include <fstream>
#include <string>
#include "conio.h"

using namespace std;

/*************************************
* DECLARACION DE TIPOS Y CONSTANTES *
*************************************/
const int MAX = 50;
const int MAXH = 10;
const int MAXE = 100;

typedef enum { Libre, Muro, DestinoL, DestinoC, DestinoJ, Jugador, Caja }tCasilla;
typedef tCasilla tTablero[MAX][MAX];
typedef struct {
	tTablero tablero;
	int nFilas, nColumnas;
	int filaJ, columnaJ;
	int nCajas;
	int nCajasColocadas;
}tSokoban;
typedef struct{
	tSokoban tableros[MAXH];
	int cont;
} tHistoria;
typedef struct {
	tSokoban estado;
	tHistoria historia;
	int numMovimientos;
	string nFichero;
	int nivel;
} tJuego;
typedef struct {
	string nombreFichero;
	int nivel;
	int minMovimientos;
}tPartida;
typedef tPartida tExitos[MAXE];
typedef struct {
	string nombreJ;
	tExitos exitos;
	int contExitos;
}tInfo;
typedef enum { Arriba, Abajo, Derecha, Izquierda, Salir, Nada, Deshacer}tTecla;
/*********************************************************************************************************
*                               DECLARACION DE METODOS Y FUNCIONES                                       *
**********************************************************************************************************/
/******************
*    VERISON 1    *
*******************/
/**
* Metodo para cargar el juego
*/
bool cargarJuego(tJuego & juego);
bool cargarNivel(ifstream & fichero, tSokoban & sokoban, int nivel);
void cargarSokoban(ifstream & fichero, tSokoban & sokoban);
bool buscarNivelEnFichero(ifstream & fichero, int nivel);
int getNumLevel(string &linea);
void leerInfoFich(istream &fichero, tInfo &info);
void ejecutarLeerFichero(tInfo &info);
/**
* Métodos para la salida por pantalla
*/
void colorFondo(int color);
void dibujaCasilla(tCasilla casilla);
tCasilla getCasilla(char caracter);
void dibujar(const tJuego &juego);
void limpiarConsola();
void mostrarError(string msg);
void mostrarInfo(const tInfo &info);
/*
* Metodos para la entrada
*/
void pedirEntero(int & entero, string msg);
void pedirString(string & str, string msg);
string pedirNombre(string msg);
tTecla leerTecla();
/*
* Metodos para inicializar el juego
*/
void inicializa(tJuego & juego);
void inicializarTablero(tTablero & tablero);
void inicializarSokoban(tSokoban & sokoban);
/*
* Metodos para el movimiento
*/
void hacerMovimiento(tJuego &juego, tTecla tecla);
bool moverJugador(tJuego &juego, int f, int c);
bool moverCaja(tJuego &juego, int filaC, int columnaC, int f, int  c);
void modificarCajaLibre(tJuego &juego, int filaC, int columnaC, int f, int c);
void modificarCajaDestino(tJuego &juego, int filaC, int columnaC, int f, int c);
void moverJugadorLibre(tJuego &juego, int f, int c);
void moverJugadorDestino(tJuego &juego, int f, int c);
bool bloqueado(tJuego & juego, int filaC, int columnaC, int dirF, int dirC);
void pruebas();
/**
* Métodos del menú:
*/
int menu();
void mostrarMenu();
int leerOpcion();
bool esOpcionValida(int opc);
void ejecutarOpc(int opc, tInfo &info);
void ejecutarJugar(tJuego & juego, tInfo &info);
/**
* Metodos para finalizar el juego
*/
void ejecutarGuardarFichero(tInfo &info);
void guardarInfoFich(ostream &fichero, tInfo &info);
int nivelExiste(tJuego &juego, tInfo &info);
bool operator < (tPartida a, tPartida b);
/*************/
/* 	Pruebas  */
/*************/
void pruebas() {

}
/********************************
*       PROGRAMA PRINCIPAL      *
*********************************/
int main() {

	int opc;				// opcion elegida por el usuario
	tInfo info;

	ejecutarLeerFichero(info);
	do {
		opc = menu();
		ejecutarOpc(opc, info);
	} while (opc != 0);

	return 0;
}
/*********************************************************************************************************
*                               IMPLEMENTACION DE METODOS Y FUNCIONES                                    *
**********************************************************************************************************/
/**
 * Funcion que busca la secuencia "level" en un string 'linea' ,
 * luego devuelve un entero , este entero el el resultado de convertir el resto de caracteres a partir de "level"
 * 
 * @param linea
 * @return 
 */
int getNumLevel(string &linea)
{
	string nLevel;
	int posEncontrado, posLevel;

	posEncontrado = linea.find("Level");
	posLevel = posEncontrado + 6;

	nLevel = linea.substr(posLevel);

	return atoi(nLevel.c_str());
}
/**
 * Procedimiento que permite modificar el color de fondo de la consola
 *
 * @param color
 */
void colorFondo(int color) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, 15 | (color << 4));
}
/**
 * Procedimiento que muestra por consola una casilla con un color definido 
 *
 * @param casilla
 */
void dibujaCasilla(tCasilla casilla) {
	
	switch (casilla)
	{
	case Libre:
		colorFondo(2);
		cout << "  ";
		break;
	case Muro:
		colorFondo(7);
		cout << "##";
		break;
	case DestinoL:
		colorFondo(14);
		cout << "..";
		break;
	case DestinoC:
		colorFondo(12); 
		cout << "..";
		break;
	case DestinoJ:
		colorFondo(0);
		cout << "00";
		break;
	case Jugador:
		colorFondo(9);
		cout << "00";
		break;
	case Caja:
		colorFondo(5);
		cout << "()";
		break;
	default:
		colorFondo(0);
		cout << "  ";
		break;
	}
	colorFondo(0);
}
/**
 * Funcion que a partir de un caracter devuelve una casilla 
 *
 * @param caracter
 * @return casilla :tCasilla
 */
tCasilla getCasilla(char caracter) {
	tCasilla casilla = Muro;

	switch (caracter)
	{
		case '#':
			casilla = Muro;
			break;
		case ' ':
			casilla = Libre;
			break;
		case '.':
			casilla = DestinoL;
			break;
		case '$':
			casilla = Caja;
			break;
		case '@':
			casilla = Jugador;
			break;
		case '*':
			casilla = DestinoC;
			break;
		case '+':
			casilla = DestinoJ;
			break;
		default:
			break;
	}
	return casilla;
}
/**
 * Procedimiento que muestra por pantalla  el 'nivel' , 'tablero' y el 'numero de movimientos' del juego 
 * @param juego
 */
void dibujar(const tJuego & juego)
{
	limpiarConsola();
	colorFondo(0);

	cout << "Fichero: " << juego.nFichero << "  Nivel: " << juego.nivel << endl;

	for (int i = 0; i < juego.estado.nFilas; i++)
	{
		for (int j = 0; j < juego.estado.nColumnas; j++)
		{
			dibujaCasilla(juego.estado.tablero[i][j]);
		}
		cout << endl;
	}

	cout << "Numero de movimientos: " << juego.numMovimientos << endl;
}
/**
 *  Procedimiento que elimiena lo que hay en la consola
 */
void limpiarConsola()
{
	system("cls");
}
/**
 * Procedimiento que muestra un error con un mensaje de color rojo durante 200 milisegundos
 *
 * @param msg
 */
void mostrarError(string msg){
	colorFondo(12);
	cout << msg << endl;
	colorFondo(0);
	Sleep(200);
}
/**
* Procedimiento que pide y comprueba si es un entero
*
* @param "msg" de tipo "string"
* @param "valor" de tipo "int"
*/
void pedirEntero(int &entero, string msg)
{
	cout << msg;
	cin >> entero;
	while (cin.fail()) {
		cin.clear();
		cin.sync();
		cout << "\tERROR: El numero introducido NO es numérico" << endl;
		cout << msg;
		cin >> entero;
	}
}
/**
* Procedimiento que pide un string
*
* @param "msg" de tipo "string"
* @param "str" de tipo "string"
*/
void pedirString(string &str, string msg)
{
	cout << msg;
	cin >> str;
}
string pedirNombre( string msg)
{
	string str;

	cout << msg;
	cin >> str;

	return str;
}
/**
 * devuelve un valor de tipo tTecla, que puede ser una de las
 cuatro direcciones si se pulsan las flechas de dirección correspondientes; 
 el valor Salir, si se pulsa la tecla Esc;
 o Nada si se pulsa cualquier otra tecla. 
 * @return 
 */
tTecla leerTecla()
{
	int dir;
	tTecla tecla = Nada;

	cin.sync();
	dir = _getch();

	if (dir == 0xe0) { // se trata de una tecla especial
		dir = _getch();

		switch (dir)
		{
		case 72:
			tecla = Arriba;
			break;
		case 80:
			tecla = Abajo;
			break;
		case 77:
			tecla = Derecha;
			break;
		case 75:
			tecla = Izquierda;
			break;
		default:
			tecla = Nada;
			break;
		}
	}

	// Si ha pulsado Esc
	if (dir == 27) {
		tecla = Salir;
	}
	if (dir == 68 || dir == 100){
		tecla = Deshacer;
	}
	return tecla;
}

/********************************************************************/
/* 	Funciones que hay que implementar que aparecen en el enunciado  */
/********************************************************************/

/**
 * Procedimiento que desplaza al jugador en funcion de los valores
 * 'dirF' y 'dirC'
 * 
 * @param juego
 * @param dirF
 * @param dirC
 * @return 
 */
bool moverJugador(tJuego &juego, int dirF, int dirC) {
	int filaJ, columnaJ;
	tCasilla siguienteCasilla;
	bool ok = false;

	columnaJ = juego.estado.columnaJ;
	filaJ = juego.estado.filaJ;

	siguienteCasilla = juego.estado.tablero[filaJ + dirF][columnaJ + dirC];
	
	if (siguienteCasilla != Muro) {

		if (moverCaja(juego, filaJ + dirF, columnaJ + dirC, dirF, dirC)) {
			moverJugadorLibre(juego, dirF, dirC);
			moverJugadorDestino(juego, dirF, dirC);
			
			juego.estado.filaJ += dirF;
			juego.estado.columnaJ += dirC;
			ok = true;
		}
	}
	return ok;
}
/**
 * Procedimiento que recoje las diferentes posibilidades de movimiento
 * para las cajas.
 * 
 * @param juego
 * @param filaC
 * @param columnaC
 * @param dirF
 * @param dirC
 * @return 
 */
bool moverCaja(tJuego & juego, int filaC, int columnaC, int dirF, int dirC)
{
	tCasilla casillaC, siguienteC;
	bool exito = true;

	casillaC = juego.estado.tablero[filaC][columnaC];
	siguienteC = juego.estado.tablero[filaC + dirF][columnaC + dirC];

	if (casillaC == Caja || casillaC == DestinoC) {
		if (siguienteC == Caja || siguienteC == Muro || siguienteC == DestinoC) {
			exito = false;
		}
		else {
			modificarCajaLibre(juego, filaC, columnaC, dirF, dirC);
			modificarCajaDestino(juego, filaC, columnaC, dirF, dirC);
			exito = true;
		}
	}
	return exito;

}
/**
 * Procedimiento con el cual se desplaza la caja, dejando 
 * su posicion original como 'Libre'.
 * 
 * @param juego
 * @param filaC
 * @param columnaC
 * @param dirF
 * @param dirC
 */
void modificarCajaLibre(tJuego & juego, int filaC, int columnaC, int dirF, int dirC)
{
	tCasilla casillaC, siguienteC;
	bool bloqueo = false;

	casillaC = juego.estado.tablero[filaC][columnaC];													
	siguienteC = juego.estado.tablero[filaC + dirF][columnaC + dirC];

	if (casillaC == Caja) {
		casillaC = Libre;
		if (siguienteC == DestinoL) {
			siguienteC = DestinoC;
			juego.estado.nCajasColocadas++;
		}
		else {
			siguienteC = Caja;
			// Metodo bloqueado
			bloqueo = bloqueado(juego, filaC, columnaC, dirF, dirC);
		}
	}

	juego.estado.tablero[filaC][columnaC] = casillaC;
	juego.estado.tablero[filaC + dirF][columnaC + dirC] = siguienteC;
}
/**
 * Recoje el desplazamiento de una caja a 'CajaDestino'
 * @param juego
 * @param filaC
 * @param columnaC
 * @param dirF
 * @param dirC
 */
void modificarCajaDestino(tJuego & juego, int filaC, int columnaC, int dirF, int dirC)
{
	tCasilla casilla, siguienteC;
	bool bloqueo = false;

	casilla = juego.estado.tablero[filaC][columnaC];
	siguienteC = juego.estado.tablero[filaC + dirF][columnaC + dirC];

	if (casilla == DestinoC) {
		casilla = DestinoL;
		if (siguienteC == DestinoL) {
			siguienteC = DestinoC;

		}
		else {
			siguienteC = Caja;
			juego.estado.nCajasColocadas --;

			// Metodo bloqueado
			bloqueo = bloqueado(juego, filaC, columnaC, dirF, dirC);

		}
	}
	juego.estado.tablero[filaC][columnaC] = casilla;
	juego.estado.tablero[filaC + dirF][columnaC + dirC] = siguienteC;
}
/**
 * Procedimiento que recoje las diferentes posibilidades de movimiento en funcion 
 * de 'dirF' y 'dirC' recibidas
 * @param juego
 * @param dirF
 * @param dirC
 */
void moverJugadorLibre(tJuego & juego, int dirF, int dirC)
{
	tCasilla casilla, siguienteC;
	int columnaJ, filaJ;
	columnaJ = juego.estado.columnaJ;
	filaJ = juego.estado.filaJ;

	casilla = juego.estado.tablero[filaJ][columnaJ];
	siguienteC = juego.estado.tablero[filaJ + dirF][columnaJ + dirC];

	if (casilla == Jugador) {
		casilla = Libre;
		if (siguienteC == DestinoL)
			siguienteC = DestinoJ;
		else
			siguienteC = Jugador;
	}
	juego.estado.tablero[filaJ][columnaJ] = casilla;
	juego.estado.tablero[filaJ + dirF][columnaJ + dirC] = siguienteC;
}
/**
 * Movimiento de un jugador entre casillas 'Destino'. Utiliza los parametros 'dirF' y 'dirC'
 * @param juego
 * @param dirF
 * @param dirC
 */
void moverJugadorDestino(tJuego & juego, int dirF, int dirC)
{
	tCasilla casilla, siguienteC;
	int columnaJ, filaJ;
	columnaJ = juego.estado.columnaJ;
	filaJ = juego.estado.filaJ;

	casilla = juego.estado.tablero[filaJ][columnaJ];
	siguienteC = juego.estado.tablero[filaJ + dirF][columnaJ + dirC];

	if (casilla == DestinoJ) {
		casilla = DestinoL;
		if (siguienteC == DestinoL)
			siguienteC = DestinoJ;
		else
			siguienteC = Jugador;
	}

	juego.estado.tablero[filaJ][columnaJ] = casilla;
	juego.estado.tablero[filaJ + dirF][columnaJ + dirC] = siguienteC;
}
/** realiza el movimiento del jugador en la dirección indicada.
 Si no se puede realizar el movimiento, no tiene efecto y 
 * no se incrementa tampoco el número de movimientos registrados
 * 
 * @param juego
 * @param tecla
 */
void hacerMovimiento(tJuego &juego, tTecla tecla){

	if (juego.historia.cont == MAXH - 1){
		juego.historia.tableros[juego.historia.cont] = juego.estado;
		juego.historia.cont = 0;
	}
	else{
		juego.historia.tableros[juego.historia.cont] = juego.estado;
		juego.historia.cont = juego.historia.cont + 1;
	}

	switch (tecla) {
		case Arriba:
			if (moverJugador(juego, -1, 0)) {
				juego.numMovimientos++;
			};
			break;
		case Abajo:
			if (moverJugador(juego, +1, 0)) {
				juego.numMovimientos++;
			};
			break;
		case Derecha:
			if (moverJugador(juego, 0, +1)) {
				juego.numMovimientos++;
			};
			break;
		case Izquierda:
			if (moverJugador(juego, 0, -1)) {
				juego.numMovimientos++;
			};
			break;
	}

}
/**
 *  inicializa el tablero, haciendo que todas las MAX x MAX
 casillas estén libres y el número de movimientos a 
 * @param tablero
 */
void inicializarTablero(tTablero &tablero) {
	for (int i = 0; i < MAX; i++)
	{
		for (int j = 0; j < MAX; j++)
		{
			tablero[i][j] = Libre;
		}
	}
}
/**
 * Procedimiento que inicializa el tablero,
 * haciendo que todas las MAX x MAX casillas estén libres y el número de movimientos a 0.
 * 
 * @param juego
 */
void inicializa(tJuego &juego) {
	// Inicializamos el estado del tablero
	inicializarTablero(juego.estado.tablero);
	//Iniciamos el contador del historial de movimientos
	juego.historia.cont = 0;
	// Modificamos el valor de numMovimientos
	juego.numMovimientos = 0;
}
/**
 * Buusca en el fichero el nivel solicitado y 
 * carga el tablero correspondiente.
 * Devuelve un booleano indicando si lo ha encontrado. 
 * @param fichero
 * @param sokoban
 * @param nivel
 * @return 
 */
bool cargarNivel(ifstream &fichero, tSokoban &sokoban, int nivel) {
	bool exito = false;

	// Inicializamos los valores del juego
	inicializarSokoban(sokoban);

	// Leemos la linea de level
	exito = buscarNivelEnFichero(fichero, nivel);

	if (exito) {
		cargarSokoban(fichero, sokoban);
	}

	return exito;
}
/**Inicializa los valores de cajas (en el mapa) y cajas
 * colocadas en el objetivo.
 * 
 * @param sokoban
 */
void inicializarSokoban(tSokoban & sokoban)
{
	sokoban.nCajas = 0;
	sokoban.nCajasColocadas = 0;
}
/**
 * Caraga el juego leyendo desde el fichero linea por linea.
 * @param fichero
 * @param sokoban
 */
void cargarSokoban(ifstream & fichero, tSokoban & sokoban)
{
	tCasilla casilla;
	string linea;
	int f = 0, c = 0;
	int colMax = 0;

	getline(fichero, linea);// primera linea del tablero

	while (linea.length() != 0) {

		for (c = 0; c < linea.length(); c++)
		{
			// Transformamos el caracter leido en una linea por un valor enumerado , devolvera una casilla
			casilla = getCasilla(linea[c]);

			if (casilla == Caja) {
				sokoban.nCajas++;
			}
			else if (casilla == DestinoC) {
				sokoban.nCajasColocadas++;
				sokoban.nCajas++;
			}
			else if (casilla == Jugador || casilla == DestinoJ)
			{
				sokoban.columnaJ = c;
				sokoban.filaJ = f;
			}

			// Guardamos la casilla en el tablero
			sokoban.tablero[f][c] = casilla;

			// Si es mayor que la maxima columna, lo guardamos
			if (c >= colMax)
				colMax = c;

		}
		f++;
		getline(fichero, linea); //siguiente linea
	}

	// Guardamos la fila y columna
	sokoban.nFilas = f;
	sokoban.nColumnas = colMax + 1;
}
/**Recorre el fichero linea por linea buscando el fichero
 * solicitado.
 * 
 * @param fichero
 * @param nivel
 * @return bool si encuentra fichero
 */
bool buscarNivelEnFichero(ifstream & fichero, int nivel)
{
	int numLevel;
	bool encontrado = false;
	string linea;

	getline(fichero, linea);	// Leemos la primera linea

	while (!fichero.eof() && !encontrado)
	{
		if (linea.find("Level") != -1) {
			numLevel = getNumLevel(linea);

			if (numLevel == nivel) {
				encontrado = true;
			}
		}
		getline(fichero, linea); // Leemos la siguiente linea
	}
	return encontrado;
}
/**Solicita al usuario el fichero y
 * el nivel que desea jugar y lo carga desde dicho fichero. 
 * 
 * @param juego
 * @return 
 */
bool cargarJuego(tJuego & juego) {
	ifstream fichero_entrada;
	string nomFichEntrada = "levels.txt";
	int nivel;
	bool exito = false;

	pedirString(nomFichEntrada, "Introduce el nombre del fichero (ej: nombre ):");
	nomFichEntrada += ".txt";

	// Abrimos el fichero
	fichero_entrada.open(nomFichEntrada);

	if (!fichero_entrada.is_open()) {
		cout << "ERROR: no se pudo abrir el archivo : " << nomFichEntrada << endl;
	}
	else {

		// Pedimos el nivel
		pedirEntero(nivel, "Introduce el nivel que desea jugar:");

		// Inicializamos el juego
		inicializa(juego);
		juego.nFichero = nomFichEntrada;
		juego.nivel = nivel;

		// Cargamos el nivel
		exito = cargarNivel(fichero_entrada, juego.estado, nivel);

		// Cerramos el fichero
		fichero_entrada.close();
	}
	return exito;
}
/** indica si alguna de las cajas
 no colocadas en destino ha quedado atrapada
 en una esquina .
 * 
 * @param juego
 * @param tecla
 * @return 
 */
bool bloqueado(tJuego & juego, int filaC, int columnaC, int dirF, int dirC){

	// Metodo Bloqueado
	tCasilla cajaA, cajaD, cajaI, cajaAB;
	bool bloqueo = false;

	// Metodo Bloqueado
	cajaA = juego.estado.tablero[filaC + dirF - 1][columnaC + dirC];
	cajaAB = juego.estado.tablero[filaC + dirF + 1][columnaC + dirC];
	cajaD = juego.estado.tablero[filaC + dirF][columnaC + 1 + dirC];
	cajaI = juego.estado.tablero[filaC + dirF][columnaC - 1 + dirC];

	if (
		(cajaA == (Muro || Caja) && cajaI == (Muro || Caja)) ||
		(cajaA == (Muro || Caja) && cajaD == (Muro || Caja)) ||
		(cajaAB == (Muro || Caja) && cajaI == (Muro || Caja)) ||
		(cajaAB == (Muro || Caja) && cajaD == (Muro || Caja))
		)

	{
		bloqueo = true;
		mostrarError("Hay una caja Esquina, si desea deshacer el movimiento pulse la letra (d o D)");
	}
	return bloqueo;
}

/**
* Funcion que devuelve una opcion del menu
* @return opc: int
*/
int menu() {
	int opc;

	mostrarMenu();
	opc = leerOpcion();

	return opc;
}
/**
* Procedimiento que muestra el menu
*/
void mostrarMenu() {
	
	cout << "		***********" << endl;
	cout << "		*  MENU   *" << endl;
	cout << "		***********" << endl;
	cout << "1.- Jugar partida" << endl;
	cout << "2.- Mostrar informacion jugador" << endl;
	cout << "0.- Salir" << endl;
	cout << "		Opcion: ";

}
/**
* Funcion que pide y valida al usuario una opcion
*
* @return opc: int
*/
int leerOpcion() {
	int opc;
	cin >> opc;

	if (cin.fail() || !esOpcionValida(opc)) { // Comprobaci�n de error de lectura
		opc = -1;
		cin.clear();
	};
	cin.sync(); // Limpiar todo lo que dejo de m�s el usuario

	return opc;
}
/**
* Funcion booleana que comprueba si una opcion es valida
*
* @param  opc: int (que representa una opcion)
* @return true: si es 'opcion >= 0 y opcion <= 2'
*/
bool esOpcionValida(int opc) {
	return (opc >= 0) && (opc <= 2);
}
/**Método que ejecuta la opción pedida o, 
de no recibir opción válida, sale.
*
* @param opc: int
* @param juego: tJuego
* @param info
*/
void ejecutarOpc(int opc, tInfo &info) {
	bool exito = false;
	tJuego  juego;

	limpiarConsola();
	switch (opc) {
		case 1:
			ejecutarJugar(juego, info);
			break;
		case 2:
			mostrarInfo(info);
			break;

		default:
			break;
	}


}/*
 * Procedimiento que dibuja el tablero a cada movimiento 
 * mientras que no se pulse 'Salir'. Cuando acaba la partida
 * comprueba si el jugador ya ha jugado ese nivel, si no lo ha hecho, guarda los datos
 * de la partida en el fichero 'jugador.txt' y, de haber jugado, sobrescribirá el numero de movimientos solamente
 * si este es menor que el ya guardado.
 * 
 *  @param juego
 *  @param info
 * */
void ejecutarJugar(tJuego & juego, tInfo &info)
{
	tTecla tecla;
	int contDeshacer = 0;
	bool partidaTerminada = false;

	if (cargarJuego(juego)) {
		do {
			// Mostramos el juego
			dibujar(juego);

			// Esperamos a que el usuario pulse una tecla
			tecla = leerTecla();

			if (tecla != Salir) {
				if (tecla == Deshacer){
					if (contDeshacer != MAXH){

						if (juego.historia.cont == 0){
							juego.estado = juego.historia.tableros[MAXH - 1];
							juego.historia.cont = MAXH - 1;
							mostrarError("DESHACER");
						}
						else{
							juego.estado = juego.historia.tableros[juego.historia.cont - 1];
							juego.historia.cont = juego.historia.cont - 1;
							mostrarError("DESHACER");
						}
						contDeshacer++;
					}
					else{
						mostrarError("Solo puede deshacer 10 veces.");
					}
				}
				else {
					hacerMovimiento(juego, tecla);
				}
			}
			
			
		} while (tecla != Salir && juego.estado.nCajasColocadas != juego.estado.nCajas);
		
		//  Mostramos el ultimo estado
		dibujar(juego);
		//guardar datos de partida en funcion de si ya se ha jugado el mapa o no
		//si NO se ha jugado el mapa

		bool terminada = (juego.estado.nCajasColocadas == juego.estado.nCajas);

		if (terminada){


			int indiceNivel = nivelExiste(juego, info);
			
			// No existe nivel
			if (indiceNivel == -1){

				//insertar linea al final
				tPartida  partida;

				partida.minMovimientos = juego.numMovimientos;
				partida.nivel = juego.nivel;
				partida.nombreFichero = juego.nFichero;
				info.exitos[info.contExitos] = partida;
				info.contExitos++;

				//Ordenar info
				int pos = info.contExitos - 1;
				tPartida dato, tmp;

				while ((pos > 0) && (info.exitos[pos ] < info.exitos[pos - 1]))
				{
					tmp = info.exitos[pos];
					info.exitos[pos] = info.exitos[pos - 1];
					info.exitos[pos - 1] = tmp;
					pos--;
				}

				//ordenarInfo(info)

				// Guardamos
				ejecutarGuardarFichero(info);
			}
			else{// Existe nivel
				// compara mov
				if (info.exitos[indiceNivel].minMovimientos > juego.numMovimientos){
					//sustituye mov
					info.exitos[indiceNivel].minMovimientos = juego.numMovimientos;
					
					// Guardamos
					ejecutarGuardarFichero(info);
				}
			}

		}

	
	}
	else {
		mostrarError("ERROR: No se ha podido cargar el juego");
	}
}
/*
 * Comprueba que si ya hay datos del nivel jugado en el fichero 'jugador.txt'.
 * 
* @param juego
* @param info
*/
int nivelExiste(tJuego &juego, tInfo &info){
	bool existe = false;
	int i = 0;
	while ( (i < info.contExitos) && !existe ){
		// ### añadir comparacion con nobre de fichcero
		if (juego.nivel == info.exitos[i].nivel && juego.nFichero == info.exitos[i].nombreFichero){
			existe = true;
		}
		else{
			i++;
		}
	}
	if (!existe) i = -1;

	return i;
}
/*Método que muestra el fichero 'jugador.txt'
 * 
 * @param info
*/
 void mostrarInfo(const tInfo &info){
	for (int i = 0; i < info.contExitos; i++){
		cout << info.exitos[i].nombreFichero << "\t";
		cout << info.exitos[i].nivel << "\t";
		cout << info.exitos[i].minMovimientos << endl;
	}
}
/*Recibe el fichero los datos de la partida: nombrefichero, nivel y min movimientos.
 * Aumenta el contador en uno.
 * @param fichero 
 * @param info
*/
void leerInfoFich(istream &fichero, tInfo &info){
	
	while (info.contExitos < 100 && !fichero.eof()){
		fichero >> info.exitos[info.contExitos].nombreFichero;
		fichero >> info.exitos[info.contExitos].nivel;
		fichero >> info.exitos[info.contExitos].minMovimientos;
		
		info.contExitos++;// Siguiente partida
	}
	
}
/* Pide al usuario su nombre y abre un fichero 'nombre.txt'. Si no existe, lo crea y si
 * no lo abre, devuelve un error.
 * Lee el archivo y lo cierra.
 * 
 * @param info
*/
void ejecutarLeerFichero(tInfo &info)
{
	ifstream archivo_entrada;

	info.nombreJ = pedirNombre("Introduce tu nombre: ");
	info.contExitos = 0;

	archivo_entrada.open(info.nombreJ + ".txt");
	

	if (!archivo_entrada.is_open()) {
		//archivo_salidaJ.close();
		mostrarError("ERROR: no se ha podido abrir el fichero " + info.nombreJ);
	}
	else {
		leerInfoFich(archivo_entrada, info);
		archivo_entrada.close();
	}

}
/*Escribe los datos de la partida en 'fichero' y salta de linea.
 * 
 * @param fichero
 * @param info
*/
void guardarInfoFich(ostream &fichero, tInfo &info){
	int i = 0;
	while (i < info.contExitos){
		fichero << info.exitos[i].nombreFichero <<" ";
		fichero << info.exitos[i].nivel <<" ";
		fichero << info.exitos[i].minMovimientos ;

		i++;// Siguiente partida
		
		if (i < info.contExitos)
			fichero << endl;
	}

}
/*
 * Guarda el fichero en 'nombre.txt'
 * @param info
*/
void ejecutarGuardarFichero(tInfo &info)
{
	ofstream archivo_salidaJ;

	archivo_salidaJ.open(info.nombreJ + ".txt");

	if (archivo_salidaJ.is_open()) {
		guardarInfoFich(archivo_salidaJ, info);
	}
	archivo_salidaJ.close();
}
/*
 * Método que modifica el funcionamiento de '<' para los parametros tPartida
 * @param tPartida a y b
 * */
bool operator < (tPartida a, tPartida b){
	return a.nombreFichero < b.nombreFichero;
}
