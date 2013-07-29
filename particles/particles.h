struct Point {
	float xVel;
	float yVel;
	float x;
	float y;
	float z;
	float color[3];
	float lifetime;
};

float GLOB = 0.0f;

class Particles {

public:

	vector<Point> points;
	float x;
	float y;
	float z;

	float time;
	float ticInterval;
	float spawnTics;
	float spawnInterval;

	float yGravity;
	float maxLifetime;
	float maxParticles;

	Particles() : time(0), spawnTics(0) {
		x = WINDOW_WIDTH/2;
		y = WINDOW_HEIGHT - 150;
		z = 0;
		ticInterval = 1.0f/30.0f; // 30 ticks per second
		spawnInterval = 1; // 1 particle per "1" tic
		yGravity = 400.0f;
		maxLifetime = 3.0f;
		maxParticles = 300;
	};

	// s = 1.0f (1 second);
	void Update(float s) {

		float tics = floor( (time+s) / ticInterval);
		if (tics < 1) {
			time+=s;
			return;
		}

		// Eliminate any particles that won't be rendered this frame
		vector<int> expiredPoints;
		for (unsigned int i = 0; i < points.size(); ++i) {
			points[i].lifetime += tics*ticInterval;
			if (points[i].lifetime > maxLifetime) expiredPoints.push_back(i);
		}
		for (unsigned int i = expiredPoints.size(); i > 0; --i) {
			points.erase(points.begin()+expiredPoints[i-1]);
		}

		for (unsigned int i = 0; i < tics; ++i) {
			UpdatePoints();

			spawnTics++;
			if ( spawnTics >= spawnInterval ) {
				if (points.size() >= maxParticles) continue;
				spawnTics = 0;
				float color[3];
				color[0] = (rand()%256)/255.0f;
				color[1] = (rand()%256)/255.0f;
				color[2] = (rand()%256)/255.0f;
				AddPoint(color);
			}
		}

		time = (time+s) - (tics*ticInterval);
	}

	int NumPoints() {
		return points.size();
	}

	vector<Point> GetPoints() {
		return points;
	}

private:

	void UpdatePoints() {
		for (unsigned int i = 0; i < points.size(); ++i) {
			points[i].x += (ticInterval*points[i].xVel);
			points[i].y += (ticInterval*points[i].yVel);
			points[i].z = i*0.01f;
			points[i].yVel += (ticInterval*yGravity);
		}
	}

	void AddPoint(float (&color)[3]) {
		Point p;

		float startXVel = (rand()%301)-150;
		//float startYVel = (rand()%1301)-1300;
		float startYVel = -600;

		p.xVel = startXVel;
		p.yVel = startYVel;
		p.x = x;
		p.y = y;
		p.z = points.size()*0.01f;
		p.color[0] = color[0];
		p.color[1] = color[1];
		p.color[2] = color[2];
		p.lifetime = 0;

		points.push_back(p);
	}
};