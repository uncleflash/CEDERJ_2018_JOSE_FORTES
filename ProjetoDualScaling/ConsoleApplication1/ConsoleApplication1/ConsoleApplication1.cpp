// ConsoleApplication1.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "stdafx.h"
#include <windows.h>
#include <Eigen/Core>
#include "dual_scaling.h"
#include "leitorbasenumerica.h"


const double LIMITE_INFERIOR_DELTA = 1e-8; // using for eliminate irrelevant dimension
typedef double real_type;
typedef Eigen::Matrix<real_type, Eigen::Dynamic, Eigen::Dynamic>	dynamic_size_matrix;
typedef Eigen::Matrix<real_type, Eigen::Dynamic, 1>					column_vector_type;
typedef Eigen::Matrix<real_type, 1, Eigen::Dynamic>					row_vector_type;
typedef ds::multiple_choice_data<dynamic_size_matrix>				mcd;


dynamic_size_matrix calculateDistanceMatrixChiSquare(dynamic_size_matrix& x_projected , row_vector_type& rho, row_vector_type& fc, column_vector_type& fr) {

	const int qtdItens = x_projected.rows();
	const int qtdDimension = x_projected.cols();

	dynamic_size_matrix distance_matrix;
	distance_matrix.resize(qtdItens, qtdItens);
	distance_matrix = Eigen::MatrixXd::Zero(qtdItens, qtdItens);

	for (int i = 0; i < qtdItens; i++) {
		for (int j = 1 + i; j < qtdItens; j++) {
			double quadradoDistancia = 0.0;
			for (int aux = 0; aux < qtdDimension; aux++) {
				double delta = (x_projected(i, aux) / sqrt(fc(i) / fr.size())) - (x_projected(j, aux) / sqrt(fc(j) / fr.size()));
				quadradoDistancia += (delta * delta)*rho(aux);
			}
			distance_matrix(i, j) = quadradoDistancia;
			distance_matrix(j, i) = distance_matrix(i, j);
		}
	}

	return distance_matrix;
}

int main(int argc, char* argv[]) {
    
	if (argc != 2) {
		std::cout << "parameter not found." << std::endl;
		std::cout << "File *.txt " << std::endl;
		return 1;
	}
	
	std::string strFile = argv[1];

	BASE_NUM base = BASE_NUM();

	if (!LeitorBaseNumerico::obterDadoArquivo(strFile, base)) {
		std::cout << "Erro read file!" << std::endl;
		return 1;
	}

	// dualscaling atributes
	dynamic_size_matrix x_normed, x_projected;
	dynamic_size_matrix y_normed, y_projected;
	row_vector_type rho, delta, fc;
	column_vector_type fr;

	// distance metric (chi_square) , distance for each par of item
	dynamic_size_matrix distance_matrix;

	ds::dual_scaling(mcd(base.getMatrix()), x_normed, y_normed, x_projected, y_projected, rho, delta, fc, fr, LIMITE_INFERIOR_DELTA);

	distance_matrix = calculateDistanceMatrixChiSquare(x_projected, rho, fc, fr);

	CreateDirectory(L"Output", NULL);

	ofstream file("Output\\xproject.txt");
		file << x_projected;
	file.close();

	file.open("Output\\fc.txt");
		file << fc;
	file.close();

	file.open("Output\\delta.txt");
		file << delta;
	file.close();

	file.open("Output\\distanceMatrix.txt");
		file << distance_matrix;
	file.close();
	
	return 0;
}

