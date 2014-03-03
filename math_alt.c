double exp(double x) {
	double N = 0;
	double S = 0;
	double Sp;
	double A = 1;
	do {
		Sp = S;
		S += A;
		N++;
		A *= x / N;
	} while (Sp != S);
	return S;
}
double pow(double x,double y); {
	return 0;
}
