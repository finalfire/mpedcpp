#include "SimulatedAnnealing.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;

void SimulatedAnnealing::randomNext(unsigned short* n1, unsigned short* n2) {
	random_shuffle(n1, n1 + this->mped->getSgl1());
	random_shuffle(n2, n2 + this->mped->getSgl2());
}

void SimulatedAnnealing::computeAndAlign() {
	this->compute();
	this->mped->computeExternalAlignment(this->computed_sigma1, this->computed_sigma2);
}

// that's the random-restart steepest ascent hill climbing
void SimulatedAnnealing::compute() {
	srand(time(NULL));

	// sigma & strings lengths
	size_t sgl1 = this->mped->getSgl1();
	size_t sgl2 = this->mped->getSgl2();
	size_t l1 = this->mped->getL1();
	size_t l2 = this->mped->getL2();

	unsigned current_dist = l1 + l2;
	unsigned best_dist = current_dist, dist = 0;
	unsigned next_dist = this->mped->computeEditDistance();

	// random sigmas
	unsigned short* sigma1_n = new unsigned short[sgl1]; copy(this->mped->getSigma1(), this->mped->getSigma1() + sgl1, sigma1_n);
	unsigned short* sigma2_n = new unsigned short[sgl2]; copy(this->mped->getSigma2(), this->mped->getSigma2() + sgl2, sigma2_n);
	// local minimum
	unsigned short* sigma1_c = new unsigned short[sgl1]; copy(this->mped->getSigma1(), this->mped->getSigma1() + sgl1, sigma1_c);
	unsigned short* sigma2_c = new unsigned short[sgl2]; copy(this->mped->getSigma2(), this->mped->getSigma2() + sgl2, sigma2_c);
	// global minimum
	unsigned short* sigma1_b = new unsigned short[sgl1]; copy(this->mped->getSigma1(), this->mped->getSigma1() + sgl1, sigma1_b);
	unsigned short* sigma2_b = new unsigned short[sgl2]; copy(this->mped->getSigma2(), this->mped->getSigma2() + sgl2, sigma2_b);

	int temperature = (l1 * sgl1) + (l2 * sgl2);
	while (temperature) {
		// TODO: define coherent decrement for temperature
		temperature--;
		// stop if temperature is zero
		if (!temperature) break;

		// TODO: which shuffling algorithm implements random_shuffle?
		// random shuffle
		randomNext(sigma1_n, sigma2_n);

		// compute the new distance
		next_dist = this->mped->computeExternalEditDistance(this->mped->get_Sigma1(), this->mped->get_Sigma2(),
				sigma1_n, sigma2_n, mped->getS1(), mped->getS2());

		// switch to the next state if dist is better
		// otherwise switch with probability e^(-deltaE/T)
		if ((current_dist > next_dist) || exp(-(next_dist - current_dist) / temperature) > rand()) {
			copy(sigma1_n, sigma1_n + sgl1, sigma1_c);
			copy(sigma2_n, sigma2_n + sgl2, sigma2_c);

			dist = next_dist;
			current_dist = next_dist;

			if (dist < best_dist) {
				best_dist = dist;
				copy(sigma1_c, sigma1_c + sgl1, sigma1_b);
				copy(sigma2_c, sigma2_c + sgl2, sigma2_b);
			}

		}
	}

	// finalize, best_dist is our minimum distance and sigma(1|2)_b is the matching schema
	this->computed_sigma1 = new unsigned short[sgl1];
	copy(sigma1_b, sigma1_b + sgl1, this->computed_sigma1);
	this->computed_sigma2 = new unsigned short[sgl2];
	copy(sigma2_b, sigma2_b + sgl2, this->computed_sigma2);

	this->result = best_dist;
}


