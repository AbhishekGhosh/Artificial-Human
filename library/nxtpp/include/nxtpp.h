// NXT 2.0 facade helper header

class NxtPP {
public:
	NxtPP();
	~NxtPP();

public:
	bool create();
	void destroy();

	void motorSetPower( int motor123 , int power100 );
	void motorStop( int motor123 , bool brake );
	void motorBrake( int motor123 , bool brake );
	void motorRotate( int motor123 , int power100 , int rotateDegrees , bool brake );
	int motorRotateDone( int motor123 );

private:
	void *comm; // Comm::NXTComm
	bool connected;
};
