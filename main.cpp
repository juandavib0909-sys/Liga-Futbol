#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

struct ConfigLiga {
	string nombreLiga;
	int puntosvictoria;
	int puntosderrota;
	int puntosempate;
	vector <string> equipos;
};
struct Partido {
	string fecha;
	string elocal;
	string evisitante;
	int goleslocal;
	int golesvisitante;
};

struct Equipo {
	string NombreE;
	int Pjugados = 0;
	int Pganados = 0;
	int Pempatados = 0;
	int Pperdidos = 0;
	int GolesMetidos = 0;
	int GolesRecibidos = 0;
	int DiferenciaGoles = 0;
	int PuntosTotales = 0;
};

ConfigLiga leerConfig() {
	ConfigLiga config;
	ifstream Configu("data/config.txt");

	if (!Configu.is_open()) {
		cout << "No se Abrio la configuracion (el archivo config.txt)";
		exit(1);
	}
	string linea;
	while (getline(Configu, linea)) {
		if (linea.empty() || linea[0] == '#') continue;
		size_t igual = linea.find('=');
		string titulo = linea.substr(0, igual);
		string valor = linea.substr(igual + 1);

		if (titulo == "liga") config.nombreLiga = valor;
		else if (titulo == "victoria") config.puntosvictoria = stoi(valor);
		else if (titulo == "empate") config.puntosempate = stoi(valor);
		else if (titulo == "derrota") config.puntosderrota = stoi(valor);
		else if (titulo == "equipo") config.equipos.push_back(valor);
	}

	Configu.close();
	return config;
}
 bool Equipoexiste(string nombre, ConfigLiga config) {
	 for (int i = 0; i < config.equipos.size(); i++) {
		 if (config.equipos[i] == nombre) return true;
	 }
	 return false;
 }


void guardarPartido(Partido partido) {
	ofstream par("data/partidos.txt", ios::app);

	if (!par.is_open()) {
		cout << "No se pudo abrir partidos.txt socio \n";
		return;
	}

	par << partido.fecha << "|" << partido.elocal << "|" << partido.evisitante << "|" << partido.goleslocal << "|" << partido.golesvisitante << endl;
	 par.close();

}

void guardarjornadita(int jornada, Partido partido) {
	ofstream jornadas("data/fechas.txt", ios::app);
	if (!jornadas.is_open()) return;

	jornadas << "JORNADA=" << jornada << endl;
	jornadas << partido.fecha << "|" << partido.elocal << "|" << partido.evisitante << "|" << partido.goleslocal << "|" << partido.golesvisitante << endl;
	jornadas << "Se acabo la jornada socio" << endl;
	jornadas.close();
}

vector<Partido> leerPartidos() {
	vector<Partido> partidos;
	ifstream par("data/partidos.txt");

	if (!par.is_open()) return partidos;

	string linea;
	while (getline(par, linea)) {
		stringstream ss(linea);
		Partido p;
		string temp;

		getline(ss, p.fecha, '|');
		getline(ss, p.elocal, '|');
		getline(ss, p.evisitante, '|');
		getline(ss, temp, '|'); p.goleslocal = stoi(temp);
		getline(ss, temp, '|'); p.golesvisitante = stoi(temp);

		partidos.push_back(p);
	}
	par.close();
	return partidos;

}
void updatearequipo(Equipo* eq, int GolesMetidos, int GolesRecibidos, ConfigLiga config) {
	eq->Pjugados++;
	eq->GolesMetidos += GolesMetidos;
	eq->GolesRecibidos += GolesRecibidos;
	eq->DiferenciaGoles = eq->GolesMetidos - eq->GolesRecibidos;

	if (GolesMetidos > GolesRecibidos) {
		eq->Pganados++;
		eq->PuntosTotales += config.puntosvictoria;
	}
	else if (GolesMetidos == GolesRecibidos) {
		eq->Pempatados++;
		eq->PuntosTotales += config.puntosempate;
	}
	else {
		eq->Pperdidos++;
		eq->PuntosTotales += config.puntosderrota;

	}
}

vector<Equipo> hacertablita(vector<Partido> partidos, ConfigLiga config) {
	vector<Equipo> tabla;

	for (string nombre : config.equipos) {
		Equipo eq;
		eq.NombreE = nombre;
		tabla.push_back(eq);
	}
	for (int i = 0; i < partidos.size(); i++) {
		for (int j = 0; j < tabla.size(); j++) {
			if (tabla[j].NombreE == partidos[i].elocal)
				updatearequipo(&tabla[j], partidos[i].goleslocal, partidos[i].golesvisitante, config);
			if (tabla[j].NombreE == partidos[i].evisitante)
				updatearequipo(&tabla[j], partidos[i].golesvisitante, partidos[i].goleslocal, config);
		}
	}
	return tabla;

}

bool comparazao(Equipo a, Equipo b) {
	if (a.PuntosTotales != b.PuntosTotales) return a.PuntosTotales > b.PuntosTotales;
	return a.DiferenciaGoles > b.DiferenciaGoles;
}
void mostrartablita(vector<Equipo> tabla) {
	sort(tabla.begin(), tabla.end(), comparazao);

	cout << "\n================================================================================\n";
	cout << "                         TABLA DE POSICIONES ACTUAL                             \n";
	cout << "================================================================================\n";
	cout << "Equipo\t\tPJ\tPG\tPE\tPP\tGM\tGR\tDG\tPUNTOS\n";
	cout << "--------------------------------------------------------------------------------\n";

	for (int i = 0; i < tabla.size(); i++) {
		cout << tabla[i].NombreE;

		// Si el nombre es corto, metemos dos tabs para alinear, si no, uno solo
		if (tabla[i].NombreE.length() < 8) {
			cout << "\t\t";
		}
		else {
			cout << "\t";
		}

		cout << tabla[i].Pjugados << "\t"
			<< tabla[i].Pganados << "\t"
			<< tabla[i].Pempatados << "\t"
			<< tabla[i].Pperdidos << "\t"
			<< tabla[i].GolesMetidos << "\t"
			<< tabla[i].GolesRecibidos << "\t"
			<< tabla[i].DiferenciaGoles << "\t"
			<< tabla[i].PuntosTotales << endl;
	}
	cout << "================================================================================\n";
}

void guardartablitatxt(vector<Equipo> tabla) {
	ofstream tablita("data/tabla.txt");
	if (!tablita.is_open()) {
		cout << "No se creo el archivo tabla.txt \n";
		return;
	}
	sort(tabla.begin(), tabla.end(), comparazao);
	tablita << "TABLA DE POSICIONES\n";
	tablita << "================================================================================\n";
	tablita << "Equipo\t\tPJ\tPG\tPE\tPP\tGM\tGR\tDG\tPUNTOS\n";
	tablita << "--------------------------------------------------------------------------------\n";

	for (int i = 0; i < tabla.size(); i++) {
		tablita << tabla[i].NombreE;

		
		if (tabla[i].NombreE.length() < 8) {
			tablita << "\t\t";
		}
		else {
			tablita << "\t";
		}

		tablita << tabla[i].Pjugados << "\t"
			<< tabla[i].Pganados << "\t"
			<< tabla[i].Pempatados << "\t"
			<< tabla[i].Pperdidos << "\t"
			<< tabla[i].GolesMetidos << "\t"
			<< tabla[i].GolesRecibidos << "\t"
			<< tabla[i].DiferenciaGoles << "\t"
			<< tabla[i].PuntosTotales << endl;
	}

	tablita << "================================================================================\n";
	tablita.close();
}

void mostrarhistorial() {
	ifstream historial("data/fechas.txt");
	if (!historial.is_open()) {
		cout << "no hay historial de jornadas.\n";
		return;
	}
	string linea;
	cout << "\n=== HISTORIAL DE JORNADAS ===\n";
	while (getline(historial, linea)) cout << linea << endl;
	historial.close();
}

void mostrarpartiditos() {
	vector<Partido> partidos = leerPartidos();
	cout << "\n=== PARTIDOS JUGADOS PAPI ===\n";
	for (int i = 0; i < partidos.size(); i++) {
		cout << partidos[i].fecha << " | " << partidos[i].elocal << " " << partidos[i].goleslocal << "-" << partidos[i].golesvisitante << " " << partidos[i].evisitante << endl;
	}
}
Partido registrarpar(ConfigLiga config) {
	Partido pa;
	cout << "\nEquipos Disponibles: \n";
	for (int i = 0; i < config.equipos.size(); i++) cout << "- " << config.equipos[i] << endl;

	cin.ignore();
	cout << "Fecha: "; 
	getline(cin, pa.fecha);

	do {
		cout << "Equipo Local: ";
		getline(cin, pa.elocal);
	} while (!Equipoexiste(pa.elocal, config));

	do {
		cout << "Equipo Visitante: ";
		getline(cin, pa.evisitante);
	} while (!Equipoexiste(pa.evisitante, config) || pa.evisitante == pa.elocal);

	cout << "Goles Local: ";
	cin >> pa.goleslocal;
	cout << "Goles Visitante: ";
	cin >> pa.golesvisitante;
	return pa;
}

int menu(string liga) {
	int op;
	do {
		cout << "\n--- " << liga << "---\n";
		cout << "1. ver tabla \n2. Registrar partido\n3. Ver historial jornadas\n";
		cout << "4. Ver partidos jugados\n5. Guardar tabla en txt\n6. Salir\nOpcion: ";
		cin >> op;
	} while (op < 1 || op > 6);
	return op;
}

int main() {
	ConfigLiga liga = leerConfig();
	int opcion;
	int jornada = 1;

	do {
		opcion = menu(liga.nombreLiga);
		switch (opcion) {
		case 1:
			mostrartablita(hacertablita(leerPartidos(), liga));
			break;
		case 2: {
			Partido pa = registrarpar(liga);
			guardarPartido(pa);
			guardarjornadita(jornada, pa);
			jornada++;
			break;
		}
		case 3: mostrarhistorial();
			break;
		case 4: mostrarpartiditos();
			break;
		case 5: 
			guardartablitatxt(hacertablita(leerPartidos(), liga));
			cout << "El documento Tabla.txt fue creado con exito pa. \n";
			break;
		case 6: cout << "Saliendo del increible gestionador de liga deportiva....\n";
			break;
		}
	} while (opcion != 6);

	return 0;
}