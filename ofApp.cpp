#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openFrameworks");

	ofBackground(39);
	ofSetLineWidth(0.5);
	ofEnableDepthTest();

	auto radius = 8;
	auto x_span = radius * sqrt(3);
	auto flg = true;
	for (float y = -500; y < 500; y += radius * 1.5) {

		for (float x = -500; x < 500; x += x_span) {

			glm::vec3 location;
			if (flg) {

				location = glm::vec3(x, y, 0);
			}
			else {

				location = glm::vec3(x + (x_span / 2), y, 0);
			}

			this->location_list.push_back(location);
			this->life_list.push_back(0);
			this->noise_param_list.push_back(ofRandom(1000));
			this->color_list.push_back(ofColor(255));
		}
		flg = !flg;
	}

	this->frame.setMode(ofPrimitiveMode::OF_PRIMITIVE_LINES);
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);

	this->face.clear();
	this->frame.clear();
	this->noise_location_list.clear();

	for (int i = 0; i < this->location_list.size(); i++) {

		this->life_list[i] = this->life_list[i] > 0 ? this->life_list[i] - 0.05 : 0;
		if (this->life_list[i] > 0) {

			this->noise_param_list[i] += ofMap(this->life_list[i], 0, 100, 0.01, 0.05);
		}
	}

	ofColor color;
	int deg_start = ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.008), 0, 1, 0, 720);
	int radius = ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.01), 0, 1, 0, 450);

	int hue = ofGetFrameNum() * 5;
	for (int deg = deg_start; deg < deg_start + 360; deg += 90) {

		auto noise_location = glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), 0);
		this->noise_location_list.push_back(noise_location);
		for (int i = 0; i < this->location_list.size(); i++) {

			if (glm::distance(noise_location, this->location_list[i]) < 25) {

				this->life_list[i] = this->life_list[i] < 100 ? this->life_list[i] + 8 : 100;
				color.setHsb(hue % 255, 180, 255);
				this->color_list[i] = color;
			}
		}

		hue += 64;
	}

	for (int i = 0; i < this->location_list.size(); i++) {

		auto height = 5.f;
		if (this->life_list[i] > 10) {

			height = ofMap(this->life_list[i], 10, 100, 5, 200);
			this->setHexagonToMesh(this->face, this->frame, this->location_list[i], 8, height, this->color_list[i], ofColor(255));
		}
		else if (this->life_list[i] > 0) {

			this->setHexagonToMesh(this->face, this->frame, this->location_list[i], ofMap(this->life_list[i], 0, 10, 0, 8), height, this->color_list[i], ofColor(255));
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();

	this->face.draw();
	this->frame.draw();

	this->cam.end();
}

//--------------------------------------------------------------
void ofApp::setHexagonToMesh(ofMesh& face_target, ofMesh& frame_target, glm::vec3 location, float radius, float height, ofColor face_color, ofColor frame_color) {

	for (int deg = 90; deg < 450; deg += 60) {

		auto face_radius = radius - 0.5;
		auto face_index = face_target.getNumVertices();

		vector<glm::vec3> vertices;
		vertices.push_back(location + glm::vec3(0, 0, 0));
		vertices.push_back(location + glm::vec3(face_radius * cos(deg * DEG_TO_RAD), face_radius * sin(deg * DEG_TO_RAD), 0));
		vertices.push_back(location + glm::vec3(face_radius * cos((deg + 60) * DEG_TO_RAD), face_radius * sin((deg + 60) * DEG_TO_RAD), 0));
		vertices.push_back(location + glm::vec3(0, 0, height));
		vertices.push_back(location + glm::vec3(face_radius * cos((deg + 60) * DEG_TO_RAD), face_radius * sin((deg + 60) * DEG_TO_RAD), height));
		vertices.push_back(location + glm::vec3(face_radius * cos(deg * DEG_TO_RAD), face_radius * sin(deg * DEG_TO_RAD), height));

		face_target.addVertices(vertices);

		face_target.addIndex(face_index + 0); face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2);
		face_target.addIndex(face_index + 3); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);
		face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 2); face_target.addIndex(face_index + 4);
		face_target.addIndex(face_index + 1); face_target.addIndex(face_index + 4); face_target.addIndex(face_index + 5);

		auto frame_index = frame_target.getNumVertices();

		vertices.clear();
		vertices.push_back(location + glm::vec3(0, 0, 0));
		vertices.push_back(location + glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), 0));
		vertices.push_back(location + glm::vec3(radius * cos((deg + 60) * DEG_TO_RAD), radius * sin((deg + 60) * DEG_TO_RAD), 0));
		vertices.push_back(location + glm::vec3(0, 0, height));
		vertices.push_back(location + glm::vec3(radius * cos((deg + 60) * DEG_TO_RAD), radius * sin((deg + 60) * DEG_TO_RAD), height));
		vertices.push_back(location + glm::vec3(radius * cos(deg * DEG_TO_RAD), radius * sin(deg * DEG_TO_RAD), height));

		frame_target.addVertices(vertices);

		frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 2);
		frame_target.addIndex(frame_index + 4); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 1); frame_target.addIndex(frame_index + 5);
		frame_target.addIndex(frame_index + 2); frame_target.addIndex(frame_index + 4);

		for (int i = 0; i < vertices.size(); i++) {

			face_target.addColor(face_color);
			frame_target.addColor(frame_color);
		}
	}
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}