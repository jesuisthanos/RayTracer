#ifndef GOOCH_PARAMETERS_H
#define GOOCH_PARAMETERS_H

class GoochParameters
{
public:
	double b;
	double y;
	double alpha;
	double beta;
	bool outline;

	GoochParameters(){
		outline = false;
	}
};

#endif /* GOOCH_PARAMETERS_H */