#include "ofxDigitalDanceBvh.h"

using namespace mlib;

int ofxDigitalDanceBvh::getNumFrames()
{
    mNumFrames = this->num_frames;
    return mNumFrames;
}

int ofxDigitalDanceBvh::getFrameSize()
{
    return frames.size();
}

void ofxDigitalDanceBvh::update()
{
    frame_new = false;
    
    if (playing && ofGetFrameNum() > 1)
    {
        int last_index = getFrame();
        
        play_head += ofGetLastFrameTime() * rate;
        int index = getFrame();
        
        if (index != last_index)
        {
            need_update = true;
            
            if(index < frames.size())
                currentFrame = frames[index];
            
            if (index >= frames.size())
            {
                if (loop)
                    play_head = 0;
                else
                    playing = false;
            }
            
            if (play_head < 0)
                play_head = 0;
        }
    }
    
    if (need_update)
    {
        need_update = false;
        frame_new = true;
        
        int index = 0;
        updateJoint(index, currentFrame, root);
    }
}


void ofxDigitalDanceBvh::drawPerfume()
{
    for (int i = 0; i < joints.size(); i++)
    {
        ofxBvhJoint *o = joints[i];
        
        if (o->isSite())
        {
        }
        else if (o->getChildren().size() == 1)
        {
            if(i<6 || (i>7 && i<9) || i==13){
                ofSetColor(ofColor::red);
            }
            else if(i>16 && i<31){
                ofSetColor(ofColor::blue);
            }
            else{
                ofSetColor(ofColor(225, 165, 135));
            }
            
            //ofDrawLine(o->getPosition(), o->getChildren()[0]->getPosition());
            if(!o->getChildren()[0]->isSite()){
                if(i==9 || i==14){
                    ofSetColor(ofColor::red);
                }
                ofDrawSphere(o->getPosition().x, o->getPosition().y, o->getPosition().z, 4.0f);
            }
            
            if(i==1|| i==2 || i==3){
                ofSetColor(ofColor::red);
                this->drawElipsoid(o->getPosition(), o->getChildren()[0]->getPosition(), 0.8);
            }
            else
                this->drawElipsoid(o->getPosition(), o->getChildren()[0]->getPosition(), 0.3);
        }
        else if (o->getChildren().size() > 1)
        {
            if(i==0)
                ofSetColor(ofColor::blue);
            else
                ofSetColor(ofColor::red);
            
            
            ofDrawSphere(o->getPosition().x, o->getPosition().y, o->getPosition().z, 3.0f);
            
            for(int i=0; i<o->getChildren().size(); i++){
                //ofDrawLine(o->getPosition(), o->getChildren()[i]->getPosition());
                this->drawElipsoid(o->getPosition(), o->getChildren()[i]->getPosition(), 1.0);
            }
        }
        glPopMatrix();
    }
    
    ofSetColor(ofColor::white);
}

void ofxDigitalDanceBvh::draw()
{
    ofPushStyle();
    for (int i = 0; i < joints.size(); i++)
    {
        ofxBvhJoint *o = joints[i];
        
        if (o->isSite())
        {
        }
        else if (o->getChildren().size() == 1)
        {
            ofDrawLine(o->getPosition(), o->getChildren()[0]->getPosition());
        }
        else if (o->getChildren().size() > 1)
        {
            for(int i=0; i<o->getChildren().size(); i++){
                ofDrawLine(o->getPosition(), o->getChildren()[i]->getPosition());
            }
        }
        glPopMatrix();
    }
    ofPopStyle();
    ofSetColor(ofColor::white);
}

void ofxDigitalDanceBvh::drawElipsoid()
{
    for (int i = 0; i < joints.size(); i++)
    {
        ofxBvhJoint *o = joints[i];
        
        if (o->isSite())
        {
        }
        else if (o->getChildren().size() == 1)
        {
            this->drawElipsoid(o->getPosition(), o->getChildren()[0]->getPosition(), 0.3);
        }
        else if (o->getChildren().size() > 1)
        {
            for(int i=0; i<o->getChildren().size(); i++){
                this->drawElipsoid(o->getPosition(), o->getChildren()[i]->getPosition(), 0.5);
            }
        }
        glPopMatrix();
    }
    ofSetColor(ofColor::white);
}

float ofxDigitalDanceBvh::getFrameTime()
{
    return frame_time;
}

void ofxDigitalDanceBvh::setTransRotate(ofVec3f root_pos, ofQuaternion *q)
{
	FrameData f;
	f.push_back(root_pos.x);
	f.push_back(root_pos.y);
	f.push_back(root_pos.z);

	for (int i = 0; i<joints.size(); i++) {
		if (getJoint(i)->getName() != "Site") {
			ofMatrix4x4 mat = this->getJoint(i)->getMatrix();
			ofQuaternion qut;
			qut.set(mat);
			ofVec3f euler = qut.getEuler();
			f.push_back(euler.z);
			f.push_back(euler.x);
			f.push_back(euler.y);
		}
	}
	frames.push_back(f);
	this->num_frames = frames.size();
}

void ofxDigitalDanceBvh::motionInitialize()
{
	frames.clear();
}

void ofxDigitalDanceBvh::childroop(const ofxBvhJoint* joint, string str) {
	for (int c = 0; c < joint->getChildren().size(); c++) {
		// �eJOINT�̏�������
		if (joint->getChildren().at(c)->getChildren().size() > 0) {
			// JOINT�̍s������
			ofs << str << "JOINT " << joint->getChildren().at(c)->getName() << endl;

			// "{"������
			ofs << str << "{" << endl;

			//�X�y�[�X�̐��𑝂₷
			str += " ";

			// OFFSET�̍s������
			ofVec3f off = joint->getChildren().at(c)->getOffset();
			ofs << str << "OFFSET " << fixed << setprecision(6) << off.x << " " << off.y << " " << off.z << endl;

			// CHANNELS�̍s������
			ofs << str << "CHANNELS 3 Yrotation Xrotation Zrotation" << endl;

			// �ċA
			childroop(joint->getChildren().at(c), str);

			// space���炷
			str.erase(str.find_last_of(" "));
		}
		else {
			// END�̍s������
			ofs << str << "End " << joint->getChildren().at(c)->getName() << endl;

			// "{"������
			ofs << str << "{" << endl;

			//�X�y�[�X�̐��𑝂₷
			string str2 = str + " ";

			// OFFSET�̍s������
			ofVec3f off = joint->getChildren().at(c)->getOffset();
			ofs << str2 << "OFFSET " << fixed << setprecision(6) << off.x << " " << off.y << " " << off.z << endl;
		}
		// �q�����Ȃ��Ȃ�� "}" ������
		ofs << str << "}" << endl;
	}
}


void ofxDigitalDanceBvh::exportBvh(string filename) {
	string str = " ";
	ofs.open(filename);
	//HIERARCHY
	ofs << "HIERARCHY" << endl;
	// ROOT�̏�������
	ofs << "ROOT " << getJoint(0)->getName() << endl;
	ofs << "{" << endl;
	double roff = 0.000000;
	ofs << str << "OFFSET " << fixed << setprecision(6) << roff << " " << roff << " " << roff << endl;
	ofs << str << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation" << endl;
	childroop(getJoint(0), str);
	ofs << "}" << endl;

	//MOTION
	ofs << "MOTION" << endl;
	ofs << "Frames: " << num_frames << endl;
	ofs << "Frame Time: " << fixed << setprecision(6) << frame_time << endl;

	for (int i = 0; i<this->num_frames; i++)
	{
		//this->update();
		int j = 0;
		for (j = 0; j<frames.at(i).size() - 1; j++) {
			ofs << frames.at(i).at(j) << " ";
		}
		ofs << fixed << frames.at(i).at(j) << std::endl;
	}

	ofs.close();
}


void ofxDigitalDanceBvh::childroopScaledOffset(const ofxBvhJoint* joint, string str, float scale) {
	for (int c = 0; c < joint->getChildren().size(); c++) {
		// �eJOINT�̏�������
		if (joint->getChildren().at(c)->getChildren().size() > 0) {
			// JOINT�̍s������
			ofs << str << "JOINT " << joint->getChildren().at(c)->getName() << endl;

			// "{"������
			ofs << str << "{" << endl;

			//�X�y�[�X�̐��𑝂₷
			str += " ";

			// OFFSET�̍s������
			ofVec3f off = scale * joint->getChildren().at(c)->getOffset();
			ofs << str << "OFFSET " << fixed << setprecision(6) << off.x << " " << off.y << " " << off.z << endl;

			// CHANNELS�̍s������
			ofs << str << "CHANNELS 3 Zrotation Yrotation Xrotation" << endl;

			// �ċA
			childroopScaledOffset(joint->getChildren().at(c), str, scale);

			// space���炷
			str.erase(str.find_last_of(" "));
		}
		else {
			// END�̍s������
			ofs << str << "End " << joint->getChildren().at(c)->getName() << endl;

			// "{"������
			ofs << str << "{" << endl;

			//�X�y�[�X�̐��𑝂₷
			string str2 = str + " ";

			// OFFSET�̍s������
			ofVec3f off = scale * joint->getChildren().at(c)->getOffset();
			ofs << str2 << "OFFSET " << fixed << setprecision(6) << off.x << " " << off.y << " " << off.z << endl;
		}
		// �q�����Ȃ��Ȃ�� "}" ������
		ofs << str << "}" << endl;
	}
}

void ofxDigitalDanceBvh::exportScaledOffset(string filename, float scale) {
	string str = " ";
	ofs.open(filename);
	//HIERARCHY
	ofs << "HIERARCHY" << endl;
	// ROOT�̏�������
	ofs << "ROOT " << getJoint(0)->getName() << endl;
	ofs << "{" << endl;
	double roff = 0.000000;
	ofs << str << "OFFSET " << fixed << setprecision(6) << roff << " " << roff << " " << roff << endl;
	ofs << str << "CHANNELS 6 Xposition Yposition Zposition Zrotation Yrotation Xrotation" << endl;
	childroopScaledOffset(getJoint(0), str, scale);
	ofs << "}" << endl;

	//MOTION
	ofs << "MOTION" << endl;
	ofs << "Frames: " << num_frames << endl;
	ofs << "Frame Time: " << fixed << setprecision(6) << frame_time << endl;

	for (int i = 0; i<this->num_frames; i++)
	{
		int j = 0;
		for (j = 0; j<frames.at(i).size() - 1; j++) {
			if (j < 3) {
				ofs << scale * frames.at(i).at(j) << " ";
			}
			else {
				ofs << frames.at(i).at(j) << " ";
			}
		}
		ofs << fixed << frames.at(i).at(j) << std::endl;
	}

	ofs.close();
}
//
//vector<vector<double>> ofxDigitalDanceBvh::exportAngular() {
//	vector<vector<double>> ang(this->getNumFrames(), vector<double>(27));
//	for (int i = 0; i<this->num_frames; i++)
//	{
//		int countend = 0;
//		int numj = 0;
//		for (int j = 0; j<this->getNumJoints(); j++) {
//			string name = this->getJoint(j)->getName();
//			if (name == "Hips" || name == "RightShoulder" || name == "RightElbow" || name == "LeftShoulder" || name == "LeftElbow" || name == "RightHip" || name == "RightKnee" || name == "LeftHip" || name == "LeftKnee") {
//				ang[i][3 * numj] = frames.at(i).at(3 + (3 * j) - (3 * countend));
//				ang[i][(3 * numj) + 1] = frames.at(i).at(4 + (3 * j) - (3 * countend));
//				ang[i][(3 * numj) + 2] = frames.at(i).at(5 + (3 * j) - (3 * countend));
//				numj++;
//			}
//			else if (name == "Site") {
//				countend++;
//			}
//		}
//	}
//	return ang;
//}

void ofxDigitalDanceBvh::exportAngulartoCSV(string filename) {
	ofs.open(filename);
	for (int i = 0; i<this->num_frames; i++)
	{
		int countend = 0;
		for (int j = 0; j<this->getNumJoints(); j++) {
			string name = this->getJoint(j)->getName();
			if (name == "Hips" || name == "RightShoulder" || name == "RightElbow" || name == "LeftShoulder" || name == "LeftElbow" || name == "RightHip" || name == "RightKnee" || name == "LeftHip") {
				ofs << frames.at(i).at(3 + (3 * j) - (3 * countend)) << ","
					<< frames.at(i).at(4 + (3 * j) - (3 * countend)) << ","
					<< frames.at(i).at(5 + (3 * j) - (3 * countend)) << ",";
			}
			else if (name == "LeftKnee") {
				ofs << frames.at(i).at(3 + (3 * j) - (3 * countend))
					<< "," << frames.at(i).at(4 + (3 * j) - (3 * countend))
					<< "," << frames.at(i).at(5 + (3 * j) - (3 * countend));
			}

			else if (name == "Site") {
				countend++;
			}
		}
		ofs << endl;
	}
	ofs.close();
}

void ofxDigitalDanceBvh::exportAngulartoTXT(string filename) {
	ofs.open(filename);
	for (int i = 0; i<this->num_frames; i++)
	{
		int countend = 0;
		for (int j = 0; j<this->getNumJoints(); j++) {
			string name = this->getJoint(j)->getName();
			if (name == "Hips" || name == "RightShoulder" || name == "RightElbow" || name == "LeftShoulder" || name == "LeftElbow" || name == "RightHip" || name == "RightKnee" || name == "LeftHip") {
				ofs << frames.at(i).at(3 + (3 * j) - (3 * countend)) / 10.0 << " "
					<< frames.at(i).at(4 + (3 * j) - (3 * countend)) / 10.0 << " "
					<< frames.at(i).at(5 + (3 * j) - (3 * countend)) / 10.0 << " ";
			}
			else if (name == "LeftKnee") {
				ofs << frames.at(i).at(3 + (3 * j) - (3 * countend))
					<< " " << frames.at(i).at(4 + (3 * j) - (3 * countend)) / 10.0
					<< " " << frames.at(i).at(5 + (3 * j) - (3 * countend)) / 10.0;
			}

			else if (name == "Site") {
				countend++;
			}
		}
		ofs << endl;
	}
	ofs.close();
}

void ofxDigitalDanceBvh::exportAngularVelocity(string filename) {
	ofs.open(filename);
	for (int i = 1; i<this->num_frames; i++)
	{
		int countend = 0;
		for (int j = 0; j<this->getNumJoints(); j++) {
			string name = this->getJoint(j)->getName();
			if (name == "Hips" || name == "RightShoulder" || name == "RightElbow" || name == "LeftShoulder" || name == "LeftElbow" || name == "RightHip" || name == "RightKnee" || name == "LeftHip" || name == "LeftKnee") {
				ofs << "," << abs(frames.at(i).at(3 + (3 * j) - (3 * countend)) - frames.at(i - 1).at(3 + (3 * j) - (3 * countend))) / this->frame_time
					<< "," << abs(frames.at(i).at(4 + (3 * j) - (3 * countend)) - frames.at(i - 1).at(4 + (3 * j) - (3 * countend))) / this->frame_time
					<< "," << abs(frames.at(i).at(5 + (3 * j) - (3 * countend)) - frames.at(i - 1).at(5 + (3 * j) - (3 * countend))) / this->frame_time;
			}
			else if (name == "Site") {
				countend++;
			}
		}
		ofs << endl;
	}
	ofs.close();
}

// �֐ߊp�x�Ō��������v�Z
float ofxDigitalDanceBvh::getWeightEffort() {
	float weighteffort_total = 0.0;
	for (int i = 1; i<this->num_frames; i++)
	{
		int countend = 0; //bvh�t�@�C������End Site�̐����J�E���g
		int count_joint = 0; //WeightEffort�ɗp����֐ߐ��̃J�E���g
		float weighteffort_total_frame = 0.0; // �e�t���[�����Ƃ̃g�[�^��WeightEffort

		for (int j = 0; j<this->getNumJoints(); j++) {
			string name = this->getJoint(j)->getName();

			if (/*name=="Hips"||*/name == "RightShoulder" || name == "RightElbow" || name == "LeftShoulder" || name == "LeftElbow" || name == "RightHip" || name == "RightKnee" || name == "LeftHip" || name == "LeftKnee") //WeightEffort�ɗp����֐�
			{
				count_joint++;

				// �e�֐߂��Ƃ�y��, x��, z������̉�]
				weighteffort_total_frame += abs(frames.at(i).at(3 + (3 * j) - (3 * countend)) - frames.at(i - 1).at(3 + (3 * j) - (3 * countend))) / this->frame_time
					+ abs(frames.at(i).at(4 + (3 * j) - (3 * countend)) - frames.at(i - 1).at(4 + (3 * j) - (3 * countend))) / this->frame_time
					+ abs(frames.at(i).at(5 + (3 * j) - (3 * countend)) - frames.at(i - 1).at(5 + (3 * j) - (3 * countend))) / this->frame_time;
			}
			else if (name == "Site") {
				countend++;
			}
		}

		weighteffort_total = weighteffort_total_frame / (float)count_joint;
	}
	float weighteffort = weighteffort_total / (float)(this->num_frames - 1);
	return weighteffort;
}

// �֐߈ʒu�̑����Ō��������v�Z
float ofxDigitalDanceBvh::getVelAve() {
	float vel_total = 0.0;
	for (int i = 1; i<this->num_frames; i++)
	{
		int num_joint = 0; //WeightEffort�ɗp����֐ߐ��̃J�E���g
		float vel_total_frame = 0.0; // �e�t���[�����Ƃ̃g�[�^��WeightEffort

		for (int j = 0; j<this->getNumJoints(); j++) {
			string name = this->getJoint(j)->getName();
			if (/*name=="Hips"||*/name == "RightShoulder" || name == "RightElbow" || name == "LeftShoulder" || name == "LeftElbow" || name == "RightHip" || name == "RightKnee" || name == "LeftHip" || name == "LeftKnee") //WeightEffort�ɗp����֐�
			{
				// �P�t���[���Ԃ̊֐ߑ��x�i�ʒu���W�j
				setFrame(i - 1);
				update();
				ofVec3f disp = this->getJoint(j + 1)->getPosition();
				setFrame(i);
				update();
				disp = this->getJoint(j + 1)->getPosition() - disp;
				vel_total_frame += disp.length() / this->frame_time;
				num_joint++;
			}
		}
		vel_total += vel_total_frame / (float)num_joint;
	}
	float vel_ave = vel_total / (float)(this->num_frames - 1);
	return vel_ave;
}

// ���T���v�����O
void ofxDigitalDanceBvh::Resampling(const float frametime, string filename) {
	string str = " ";
	ofs.open(filename);
	//HIERARCHY
	ofs << "HIERARCHY" << endl;
	// ROOT�̏�������
	ofs << "ROOT " << getJoint(0)->getName() << endl;
	ofs << "{" << endl;
	double roff = 0.000000;
	ofs << str << "OFFSET " << fixed << setprecision(6) << roff << " " << roff << " " << roff << endl;
	ofs << str << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation" << endl;
	childroop(getJoint(0), str);
	ofs << "}" << endl;

	//MOTION
	ofs << "MOTION" << endl;
	float endtime = (num_frames - 1) * frame_time;  //�J�n�t���[������I���t���[���܂ł̎���
	const int numframes = 1 + (int)(endtime / frametime);  //���T���v�����O��̃t���[����
	ofs << "Frames: " << numframes << endl;
	ofs << "Frame Time: " << fixed << setprecision(6) << frametime << endl;

	for (int i = 0; i<numframes; i++)
	{
		float t = (float)i * frametime;
		if (t >= endtime) {
			int y_start = num_frames - 2;
			int y_stop = num_frames - 1;
			float t_start = y_start * frame_time;
			float t_stop = t_start + frame_time;
			float amt = (t - t_start) / (t_stop - t_start);
			int j = 0;
			for (j = 0; j<frames.at(0).size() - 1; j++) {
				if (j == 0 || j == 1 || j == 2) {
					float y = ofLerp(frames.at(y_start).at(j), frames.at(y_stop).at(j), amt);
					ofs << y << " ";
				}
				else {
					if (abs(frames.at(y_start).at(j) - frames.at(y_stop).at(j)) > 180.0) { //[-180,180]����[0,360]�ɕϊ����Ă�����
						if (frames.at(y_start).at(j) < 0.0) {
							float newstart = 360.0 + frames.at(y_start).at(j);
							float y = ofLerp(newstart, frames.at(y_stop).at(j), amt);
							if (y > 180.0) { //[-180,180]�ɖ߂�
								y = y - 360.0;
							}

							if (j == frames.at(0).size() - 1) {
								ofs << y << std::endl;
							}
							else {
								ofs << y << " ";
							}
						}
						else /*if(frames.at(y_stop).at(j) < 0)*/ {
							float newend = 360.0 + frames.at(y_stop).at(j);
							float y = ofLerp(frames.at(y_start).at(j), newend, amt);
							if (y > 180.0) {
								y = y - 360.0;
							}
							if (j == frames.at(0).size() - 1) {
								ofs << y << std::endl;
							}
							else {
								ofs << y << " ";
							}
						}
					}
					else {
						float y = ofLerp(frames.at(y_start).at(j), frames.at(y_stop).at(j), amt);
						if (j == frames.at(0).size() - 1) {
							ofs << y << std::endl;
						}
						else {
							ofs << y << " ";
						}
					}
				}
			}
		}
		else {
			int y_start = (int)(t / frame_time);
			int y_stop = y_start + 1;
			float t_start = y_start * frame_time;
			float t_stop = t_start + frame_time;
			float amt = (t - t_start) / (t_stop - t_start);
			int j = 0;
			for (j = 0; j<frames.at(0).size(); j++) {
				if (j == 0 || j == 1 || j == 2) {
					float y = ofLerp(frames.at(y_start).at(j), frames.at(y_stop).at(j), amt);
					ofs << y << " ";
				}
				else {
					if (abs(frames.at(y_start).at(j) - frames.at(y_stop).at(j)) > 180.0) { //[-180,180]����[0,360]�ɕϊ����Ă�����
						if (frames.at(y_start).at(j) < 0.0) {
							float newstart = 360.0 + frames.at(y_start).at(j);
							float y = ofLerp(newstart, frames.at(y_stop).at(j), amt);
							if (y > 180.0) { //[-180,180]�ɖ߂�
								y = y - 360.0;
							}

							if (j == frames.at(0).size() - 1) {
								ofs << y << std::endl;
							}
							else {
								ofs << y << " ";
							}
						}
						else /*if(frames.at(y_stop).at(j) < 0)*/ {
							float newend = 360.0 + frames.at(y_stop).at(j);
							float y = ofLerp(frames.at(y_start).at(j), newend, amt);
							if (y > 180.0) {
								y = y - 360.0;
							}
							if (j == frames.at(0).size() - 1) {
								ofs << y << std::endl;
							}
							else {
								ofs << y << " ";
							}
						}
					}
					else {
						float y = ofLerp(frames.at(y_start).at(j), frames.at(y_stop).at(j), amt);
						if (j == frames.at(0).size() - 1) {
							ofs << y << std::endl;
						}
						else {
							ofs << y << " ";
						}
					}
				}
			}
		}
	}

	ofs.close();
}

void ofxDigitalDanceBvh::InterpolatePush(string filename, ofxDigitalDanceBvh* next, int range) {
	string str = " ";
	ofs.open(filename);
	//HIERARCHY
	ofs << "HIERARCHY" << endl;
	// ROOT�̏�������
	ofs << "ROOT " << getJoint(0)->getName() << endl;
	ofs << "{" << endl;
	double roff = 0.000000;
	ofs << str << "OFFSET " << roff << " " << roff << " " << roff << endl;
	ofs << str << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation" << endl;
	childroop(getJoint(0), str);
	ofs << "}" << endl;

	//MOTION
	ofs << "MOTION" << endl;
	ofs << "Frames: " << this->num_frames + next->num_frames << endl;
	ofs << "Frame Time: " << fixed << setprecision(6) << frame_time << endl;

	for (int i = 0; i<this->num_frames; i++)
	{
		int j = 0;
		for (j = 0; j<this->frames.at(i).size() - 1; j++) {
			ofs << this->frames.at(i).at(j) << " ";
		}
		ofs << fixed << this->frames.at(i).at(j) << std::endl;
	}

	ofs.close();

	ofs.open(filename, std::ios::app);
	// frames�̍X�V
	/*ifstream ifs(filename);
	int newframes = this->num_frames + next->num_frames;
	static const int lineno = 138;
	int cnt = 0;
	string line;
	while(getline(ifs,line)){
	if(++cnt==lineno) {
	ofs << "Frames: " << newframes << endl;
	}
	else {
	ofs << line << endl;
	}
	}
	ifs.close();*/
	//this->update();
	//this->play();
	for (int i = 0; i < next->num_frames; i++)
	{
		//this->update();
		// range���ł�x,z�̈ʒu���W�␳�ƁA���̑��̂��ׂẴp�����[�^����`���
		if (i < range - 1) {
			int j = 0;
			for (j = 0; j < next->frames.at(i).size() - 1; j++) {
				if (j == 0 || j == 2) {
					// x,z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					ofs << next->frames.at(i).at(j) - (next->frames.at(0).at(j) - this->frames.at(num_frames - 1).at(j)) << " ";
				}
				else {
					// ���̑��̃p�����[�^����`���
					ofs << ofLerp(this->frames.at(this->num_frames - 1).at(j), next->frames.at(range - 1).at(j), ((float)i + 1) / range) << " ";
				}
			}
			ofs << fixed << ofLerp(this->frames.at(this->num_frames - 1).at(j), next->frames.at(range - 1).at(j), ((float)i + 1) / range)
				<< std::endl;
		}

		// range�O�ł�x,z�̈ʒu���W�␳�͂��邪�A���̑��̂��ׂẴp�����[�^�͂��̂܂܂̒l
		else {
			int j = 0;
			for (j = 0; j<frames.at(i).size() - 1; j++) {
				if (j == 0 || j == 1 || j == 2) {
					// x,z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					ofs << next->frames.at(i).at(j) - (next->frames.at(0).at(j) - this->frames.at(num_frames - 1).at(j)) << " ";
				}
				else {
					// ���̑��̃p�����[�^�͂��̂܂�
					ofs << next->frames.at(i).at(j) << " ";
				}
			}
			ofs << fixed << next->frames.at(i).at(j) << std::endl;
		}
	}
	ofs.close();

	this->unload();
	this->load(filename);
}

void ofxDigitalDanceBvh::InterpolateAttitude(string filename, ofxDigitalDanceBvh* next, int range) {
	string str = " ";
	ofs.open(filename);
	// HIERARCHY
	ofs << "HIERARCHY" << endl;
	// ROOT�̏�������
	ofs << "ROOT " << getJoint(0)->getName() << endl;
	ofs << "{" << endl;
	double roff = 0.000000;
	ofs << str << "OFFSET " << roff << " " << roff << " " << roff << endl;
	ofs << str << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation" << endl;
	childroop(getJoint(0), str);
	ofs << "}" << endl;

	// MOTION
	ofs << "MOTION" << endl;
	ofs << "Frames: " << this->num_frames + range + next->num_frames << endl;
	ofs << "Frame Time: " << fixed << setprecision(6) << frame_time << endl;

	for (int i = 0; i<this->num_frames; i++)
	{
		int j = 0;
		for (j = 0; j<this->frames.at(i).size() - 1; j++) {
			ofs << this->frames.at(i).at(j) << " ";
		}
		ofs << fixed << this->frames.at(i).at(j) << std::endl;
	}

	ofs.close();

	ofs.open(filename, std::ios::app);

	for (int i = 0; i < range; i++)
	{
		//�e�p�����[�^����`���
		int j = 0;
		for (j = 0; j < frames.at(i).size() - 1; j++) {
			// ���̑��̃p�����[�^����`���
			ofs << this->frames.at(this->num_frames - 1).at(j) + (next->frames.at(1).at(j) - this->frames.at(this->num_frames - 1).at(j)) * (((float)i + 1) / range) << " ";
		}
		ofs << fixed << ofLerp(this->frames.at(this->num_frames - 1).at(j), next->frames.at(1).at(j), ((float)i + 1) / range)
			<< std::endl;

	}

	// next�Z�O�����g�̃t���[��������������
	for (int i = 0; i<next->num_frames; i++)
	{
		int j = 0;
		for (j = 0; j<next->frames.at(i).size() - 1; j++) {
			ofs << next->frames.at(i).at(j) << " ";
		}
		ofs << fixed << next->frames.at(i).at(j) << std::endl;
	}
	ofs.close();

	this->unload();
	this->load(filename);
}

void ofxDigitalDanceBvh::LerpBVH(string filename1, string filename2, int range) {

	ofxDigitalDanceBvh bvh1, bvh2; //bvh��bvh1��bvh2���Ԃ��ĘA����������
	bvh1.load(filename1);
	bvh2.load(filename2);
	this->load(filename1);

	//�A�����̑Orange�t���[�����̏�������
	for (int i = 0; i < range - 1; i++) {
		for (int j = 0; j < this->frames.at(i).size(); j++) {
			if (j == 0 || j == 2) {
				continue;
			}
			else {
				//���̑��̃p�����[�^����`���
				if (abs(bvh1.frames.at(bvh1.num_frames - range).at(j) - bvh2.frames.at(range - 1).at(j)) > 180.0) { //[-180, 180]��[0, 360]�ɕϊ����Ă�����
					if (bvh1.frames.at(bvh1.num_frames - range).at(j) < 0.0) {
						float newstart = 360.0 + bvh1.frames.at(bvh1.num_frames - range).at(j);
						float v = ofLerp(newstart, bvh2.frames.at(range - 1).at(j), ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
						if (v > 180.0) {
							v = v - 360.0;
						}
						this->frames.at((bvh1.num_frames - 1) - i).at(j) = v;
					}
					else/* if(bvh2.frames.at(range-1).at(j) < 0)*/ {
						float newend = 360.0 + bvh2.frames.at(range - 1).at(j);
						float v = ofLerp(bvh1.frames.at(bvh1.num_frames - range).at(j), newend, ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
						if (v > 180.0) {
							v = v - 360.0;
						}
						this->frames.at((bvh1.num_frames - 1) - i).at(j) = v;
					}
				}
				else {
					float v = ofLerp(bvh1.frames.at(bvh1.num_frames - range).at(j), bvh2.frames.at(range - 1).at(j), ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
					this->frames.at((bvh1.num_frames - 1) - i).at(j) = v;
				}
			}
		}
	}

	//�A�����̌�range�t���[�����̏�������
	for (int i = 0; i < bvh2.num_frames; i++)
	{
		FrameData data; //bvh��((bvh1�̃t���[����)+(i+1))�Ԗڂ̃t���[���̃f�[�^
						// range���ł�x,z�̈ʒu���W�␳�ƁA���̑��̂��ׂẴp�����[�^����`���
		if (i < range - 1) {
			int j = 0;
			for (j = 0; j < bvh2.frames.at(i).size() - 1; j++) {
				if (j == 0 || j == 2) {
					// x, z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					float v = bvh2.frames.at(i).at(j) - (bvh2.frames.at(0).at(j) - this->frames.at(bvh1.num_frames - 1).at(j));
					data.push_back(v);
				}
				else {
					// ���̑��̃p�����[�^����`���
					// MOTION�̃p�����[�^��[-180, 180]degree�Ȃ̂ŕ␳���K�v
					if (abs(bvh1.frames.at(bvh1.num_frames - range).at(j) - bvh2.frames.at(range - 1).at(j)) > 180.0) {
						if (bvh1.frames.at(bvh1.num_frames - range).at(j) < 0.0) {
							float newstart = 360.0 + bvh1.frames.at(bvh1.num_frames - range).at(j);
							float v = ofLerp(newstart, bvh2.frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
							if (v > 180.0) {
								v = v - 360.0;
							}
							data.push_back(v);
						}
						else/* if(bvh2.frames.at(range-1).at(j) < 0)*/ {
							float newend = 360.0 + bvh2.frames.at(range - 1).at(j);
							float v = ofLerp(bvh1.frames.at(bvh1.num_frames - range).at(j), newend, ((float)range + (float)i) / (((float)range*2.0) - 1.0));
							if (v > 180.0) {
								v = v - 360.0;
							}
							data.push_back(v);
						}
					}
					else {
						float v = ofLerp(bvh1.frames.at(bvh1.num_frames - range).at(j), bvh2.frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
						data.push_back(v);
					}
				}
			}
			float v = ofLerp(bvh1.frames.at(bvh1.num_frames - range).at(j), bvh2.frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
			data.push_back(v);
		}

		// range�O�ł�x,z�̈ʒu���W�␳�͂��邪�A���̑��̂��ׂẴp�����[�^�͂��̂܂܂̒l
		else {
			int j = 0;
			for (j = 0; j<bvh2.frames.at(i).size() - 1; j++) {
				if (j == 0/*||j==1*/ || j == 2) {
					// x,z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					float v = bvh2.frames.at(i).at(j) - (bvh2.frames.at(0).at(j) - this->frames.at(bvh1.num_frames - 1).at(j));
					data.push_back(v);
				}
				else {
					// ���̑��̃p�����[�^�͂��̂܂�
					float v = bvh2.frames.at(i).at(j);
					data.push_back(v);
				}
			}
			float v = bvh2.frames.at(i).at(j);
			data.push_back(v);
		}
		frames.push_back(data);
		this->num_frames++;
	}
}

void ofxDigitalDanceBvh::LerpBVH(ofxDigitalDanceBvh* next, int range) {
	int ori_num_frames = this->num_frames;

	//�A�����̑Orange�t���[�����̏�������
	for (int i = 0; i < range - 1; i++) {
		for (int j = 0; j < this->frames.at(i).size(); j++) {
			if (j == 0 || j == 2) {
				continue;
			}
			else {
				//���̑��̃p�����[�^����`���
				ofxDigitalDanceBvh bvhh;
				bvhh.setFrame(10);
				if (abs(this->frames.at(ori_num_frames - range).at(j) - next->frames.at(range - 1).at(j)) > 180.0) {
					if (this->frames.at(ori_num_frames - range).at(j) < 0.0) {
						float newstart = 360.0 + this->frames.at(ori_num_frames - range).at(j);
						float v = ofLerp(newstart, next->frames.at(range - 1).at(j), ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
						if (v > 180.0) {
							v = v - 360.0;
						}
						this->frames.at((this->num_frames - 1) - i).at(j) = v;
					}
					else/* if(next->frames.at(range-1).at(j) < 0)*/ {
						float newend = 360.0 + next->frames.at(range - 1).at(j);
						float v = ofLerp(this->frames.at(ori_num_frames - range).at(j), newend, ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
						if (v > 180.0) {
							v = v - 360.0;
						}
						this->frames.at((this->num_frames - 1) - i).at(j) = v;
					}
				}
				else {
					float v = ofLerp(this->frames.at(ori_num_frames - range).at(j), next->frames.at(range - 1).at(j), ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
					this->frames.at((this->num_frames - 1) - i).at(j) = v;
				}
			}
		}
	}

	//�A�����̌�range�t���[�����̏�������
	for (int i = 0; i < next->num_frames; i++)
	{
		FrameData data; //bvh��((bvh1�̃t���[����)+(i+1))�Ԗڂ̃t���[���̃f�[�^
						// range���ł�x,z�̈ʒu���W�␳�ƁA���̑��̂��ׂẴp�����[�^����`���
		if (i < range - 1) {
			int j = 0;
			for (j = 0; j < next->frames.at(i).size() - 1; j++) {
				if (j == 0 || j == 2) {
					// x, z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					float v = next->frames.at(i).at(j) - (next->frames.at(0).at(j) - this->frames.at(ori_num_frames - 1).at(j));
					data.push_back(v);
				}
				else {
					// ���̑��̃p�����[�^����`���
					// MOTION�̃p�����[�^��[-180, 180]degree�Ȃ̂ŕ␳���K�v
					if (abs(this->frames.at(ori_num_frames - range).at(j) - next->frames.at(range - 1).at(j)) > 180.0) {
						if (this->frames.at(ori_num_frames - range).at(j) < 0.0) {
							float newstart = 360.0 + this->frames.at(ori_num_frames - range).at(j);
							float v = ofLerp(newstart, next->frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
							if (v > 180.0) {
								v = v - 360.0;
							}
							data.push_back(v);
						}
						else/* if(next->frames.at(range-1).at(j) < 0)*/ {
							float newend = 360.0 + next->frames.at(range - 1).at(j);
							float v = ofLerp(this->frames.at(ori_num_frames - range).at(j), newend, ((float)range + (float)i) / (((float)range*2.0) - 1.0));
							if (v > 180.0) {
								v = v - 360.0;
							}
							data.push_back(v);
						}
					}
					else {
						float v = ofLerp(this->frames.at(ori_num_frames - range).at(j), next->frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
						data.push_back(v);
					}
				}
			}
			float v = ofLerp(this->frames.at(ori_num_frames - range).at(j), next->frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
			data.push_back(v);
		}

		// range�O�ł�x,z�̈ʒu���W�␳�͂��邪�A���̑��̂��ׂẴp�����[�^�͂��̂܂܂̒l
		else {
			int j = 0;
			for (j = 0; j<next->frames.at(i).size() - 1; j++) {
				if (j == 0/*||j==1*/ || j == 2) {
					// x,z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					float v = next->frames.at(i).at(j) - (next->frames.at(0).at(j) - this->frames.at(ori_num_frames - 1).at(j));
					data.push_back(v);
				}
				else {
					// ���̑��̃p�����[�^�͂��̂܂�
					float v = next->frames.at(i).at(j);
					data.push_back(v);
				}
			}
			float v = next->frames.at(i).at(j);
			data.push_back(v);
		}
		frames.push_back(data);
		this->num_frames++;
	}
}

void ofxDigitalDanceBvh::CubicInterpolateBVH(ofxDigitalDanceBvh* next, int range) {
	int ori_num_frames = this->num_frames;

	//�A�����̑Orange�t���[�����̏�������
	for (int i = 0; i < range - 1; i++) {
		for (int j = 0; j < this->frames.at(i).size(); j++) {
			if (j == 0 || j == 2) {
				continue;
			}
			else {
				//���̑��̃p�����[�^����`���
				if (abs(this->frames.at(ori_num_frames - range).at(j) - next->frames.at(range - 1).at(j)) > 180.0) {
					if (this->frames.at(ori_num_frames - range).at(j) < 0.0) {
						float newstart = 360.0 + this->frames.at(ori_num_frames - range).at(j);
						float v = CubicInterpolate(newstart, next->frames.at(range - 1).at(j), ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
						if (v > 180.0) {
							v = v - 360.0;
						}
						this->frames.at((this->num_frames - 1) - i).at(j) = v;
					}
					else/* if(next->frames.at(range-1).at(j) < 0)*/ {
						float newend = 360.0 + next->frames.at(range - 1).at(j);
						float v = CubicInterpolate(this->frames.at(ori_num_frames - range).at(j), newend, ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
						if (v > 180.0) {
							v = v - 360.0;
						}
						this->frames.at((this->num_frames - 1) - i).at(j) = v;
					}
				}
				else {
					float v = CubicInterpolate(this->frames.at(ori_num_frames - range).at(j), next->frames.at(range - 1).at(j), ((float)range - (float)i - 1.0) / (((float)range*2.0) - 1.0));
					this->frames.at((this->num_frames - 1) - i).at(j) = v;
				}
			}
		}
	}

	//�A�����̌�range�t���[�����̏�������
	for (int i = 0; i < next->num_frames; i++)
	{
		FrameData data; //bvh��((bvh1�̃t���[����)+(i+1))�Ԗڂ̃t���[���̃f�[�^
						// range���ł�x,z�̈ʒu���W�␳�ƁA���̑��̂��ׂẴp�����[�^����`���
		if (i < range - 1) {
			int j = 0;
			for (j = 0; j < next->frames.at(i).size() - 1; j++) {
				if (j == 0 || j == 2) {
					// x, z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					float v = next->frames.at(i).at(j) - (next->frames.at(0).at(j) - this->frames.at(ori_num_frames - 1).at(j));
					data.push_back(v);
				}
				else {
					// ���̑��̃p�����[�^����`���
					// MOTION�̃p�����[�^��[-180, 180]degree�Ȃ̂ŕ␳���K�v
					if (abs(this->frames.at(ori_num_frames - range).at(j) - next->frames.at(range - 1).at(j)) > 180.0) {
						if (this->frames.at(ori_num_frames - range).at(j) < 0.0) {
							float newstart = 360.0 + this->frames.at(ori_num_frames - range).at(j);
							float v = CubicInterpolate(newstart, next->frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
							if (v > 180.0) {
								v = v - 360.0;
							}
							data.push_back(v);
						}
						else/* if(next->frames.at(range-1).at(j) < 0)*/ {
							float newend = 360.0 + next->frames.at(range - 1).at(j);
							float v = CubicInterpolate(this->frames.at(ori_num_frames - range).at(j), newend, ((float)range + (float)i) / (((float)range*2.0) - 1.0));
							if (v > 180.0) {
								v = v - 360.0;
							}
							data.push_back(v);
						}
					}
					else {
						float v = CubicInterpolate(this->frames.at(ori_num_frames - range).at(j), next->frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
						data.push_back(v);
					}
				}
			}
			float v = CubicInterpolate(this->frames.at(ori_num_frames - range).at(j), next->frames.at(range - 1).at(j), ((float)range + (float)i) / (((float)range*2.0) - 1.0));
			data.push_back(v);
		}

		// range�O�ł�x,z�̈ʒu���W�␳�͂��邪�A���̑��̂��ׂẴp�����[�^�͂��̂܂܂̒l
		else {
			int j = 0;
			for (j = 0; j<next->frames.at(i).size() - 1; j++) {
				if (j == 0/*||j==1*/ || j == 2) {
					// x,z�̈ʒu���W�͂��ׂẴt���[���ŕϊ�
					float v = next->frames.at(i).at(j) - (next->frames.at(0).at(j) - this->frames.at(ori_num_frames - 1).at(j));
					data.push_back(v);
				}
				else {
					// ���̑��̃p�����[�^�͂��̂܂�
					float v = next->frames.at(i).at(j);
					data.push_back(v);
				}
			}
			float v = next->frames.at(i).at(j);
			data.push_back(v);
		}
		frames.push_back(data);
		this->num_frames++;
	}
}



float ofxDigitalDanceBvh::getConnectivity(ofxDigitalDanceBvh* next) {
	float Sim_pose = 0.0; //�p���ގ��x
	float Sim_move = 0.0; //�����ގ��x

	for (int j = 0; j<this->getNumJoints(); j++) {
		string name_this = this->getJoint(j)->getName();

		if (name_this == "Hips" || name_this == "RightShoulder" || name_this == "RightElbow" || name_this == "LeftShoulder" || name_this == "LeftElbow" || name_this == "RightHip" || name_this == "RightKnee" || name_this == "LeftHip" || name_this == "LeftKnee") //�p����֐߂̎w��
		{
			// �O�Z�O�����g�̍ŏI�t���[���ɃZ�b�g
			this->setFrame(this->num_frames - 1);
			this->update();
			// ��Z�O�����g�̊J�n�t���[���ɃZ�b�g
			next->setFrame(0);
			next->update();

			// �Z�O�����g�Ԃ̘A�����O��1�t���[���̊e�����N�̕����x�N�g��
			ofVec3f link_this1 = this->getJoint(j + 1)->getPosition() - this->getJoint(j)->getPosition();
			ofVec3f link_next1 = next->getJoint(j + 1)->getPosition() - next->getJoint(j)->getPosition();

			// �p���ގ��xSim_pose�ɃZ�O�����g�Ԃ̘A�����O��1�t���[���̃����N�̕����x�N�g���̓��ς�Sim_pose�ɑ���
			Sim_pose += link_this1.getNormalized().dot(link_next1.getNormalized()) / 9.0f;

			// �O�Z�O�����g�̍ŏI�t���[��-1�ɃZ�b�g
			this->setFrame(this->num_frames - 2);
			this->update();
			// ��Z�O�����g�̊J�n�t���[��+1�ɃZ�b�g
			next->setFrame(1);
			next->update();

			// �Z�O�����g�Ԃ̘A�����O��2�t���[���ڂ̊e�����N�̕����x�N�g��
			ofVec3f link_this2 = this->getJoint(j + 1)->getPosition() - this->getJoint(j)->getPosition();
			ofVec3f link_next2 = next->getJoint(j + 1)->getPosition() - next->getJoint(j)->getPosition();

			ofVec3f prev_vel = (link_this1 - link_this2).getNormalized(); // �O�Z�O�����g�̍ŏI2�t���[���̑��x�i���K���j
			ofVec3f next_vel = (link_next2 - link_next1).getNormalized(); // ��Z�O�����g�̊J�n2�t���[���̑��x�i���K���j
			ofVec3f seam_vel = (link_next1 - link_this1).getNormalized(); // �A�����̑��x�i���K���j

																		  // �����ގ��xSim_move
			float g1 = seam_vel.dot(prev_vel);
			if (g1 < 0.0) {
				g1 = 0.0;
			}
			float g2 = seam_vel.dot(next_vel);
			if (g2 < 0.0) {
				g2 = 0.0;
			}
			Sim_move += (g1 * g2) / 9.0f;
		}
	}
	return Sim_pose + Sim_move;
}



void ofxDigitalDanceBvh::FillBlank(int num_seg, ofxDigitalDanceBvh bvhs[], int arraysize, int start, int goal) {

	cout << "�m�[�h�̐ݒ蒆" << endl;

	int size = (arraysize - 2) * num_seg; //�S�m�[�h�̐�-2
	int size_interseg = arraysize - 2; //bvhs�̐�-2
	std::vector<Node> nodes;
	nodes.resize(size + 2);

	//�S�m�[�h�̐ڑ����
	for (int i = 0; i < nodes.size(); i++)
	{
		if (i == start) {
			continue;
		}
		else if (i == goal) {
			nodes[i].edges_to.resize(size_interseg);
			nodes[i].edges_cost.resize(size_interseg);
			for (int j = 0; j < nodes[i].edges_to.size(); j++) {
				nodes[i].edges_to[j] = j + 2;
				//double kost = 1.0 / bvhs[j+2].getConnectivity(&bvhs[i]);
				nodes[i].edges_cost[j] = 1.0 / bvhs[j + 2].getConnectivity(&bvhs[i]);
			}
		}
		else if ((i - 2) / size_interseg == (num_seg - 1)) {
			nodes[i].edges_to.resize(1);
			nodes[i].edges_cost.resize(1);
			nodes[i].edges_to[0] = start;
			nodes[i].edges_cost[0] = 1.0 / bvhs[start].getConnectivity(&bvhs[((i - 2) % size_interseg) + 2]);
		}
		else {
			nodes[i].edges_to.resize(size_interseg);
			nodes[i].edges_cost.resize(size_interseg);
			for (int j = 0; j < nodes[i].edges_to.size(); j++) {
				nodes[i].edges_to[j] = ((((i - 2) / size_interseg) + 1) * size_interseg) + j + 2;
				/*double kost0 = bvhs[2].getConnectivity(&bvhs[2] );
				double kost = 1.0 / bvhs[j+2].getConnectivity(&bvhs[((i-2) % size_interseg) + 2 ] );*/
				if (j + 2 == ((i - 2) % size_interseg) + 2) {
					nodes[i].edges_cost[j] = 10.0;
				}
				else {
					nodes[i].edges_cost[j] = 1.0 / bvhs[j + 2].getConnectivity(&bvhs[((i - 2) % size_interseg) + 2]);
				}
			}
		}
	}

	//�S�m�[�h�̏�����
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].done = false;
		nodes[i].cost = -1.0;
	}

	//goal����T�����܂�
	nodes[goal].cost = 0.0; //�T�����߂̃m�[�h�̃R�X�g��0

							//�A���S���Y�����s
	while (true)
	{
		//�m��m�[�h��T��
		Node* doneNode = NULL; //�m��m�[�h�ɂ����邽�߂�
		int doneNode_num = NULL;
		for (int i = 0; i < nodes.size(); i++)
		{
			if (nodes[i].done || nodes[i].cost < 0)
			{
				continue;
			}
			if (doneNode == NULL || nodes[i].cost < doneNode->cost)
			{
				doneNode = &nodes[i];
				doneNode_num = i;
			}
		}
		//���m��m�[�h���Ȃ��Ȃ邩�AStart���m��m�[�h�ɂȂ�ΏI��
		if (doneNode == NULL || nodes[start].done)
		{
			break;
		}

		//�m��t���O�𗧂Ă�
		doneNode->done = true;  //����ŎQ�Ɛ�̒l����������
								//�ڑ���m�[�h�̏����X�V����
		for (int i = 0; i < doneNode->edges_to.size(); i++)
		{
			int to = doneNode->edges_to[i];
			double cost1 = nodes[1].edges_cost[i];
			double cost2 = doneNode->edges_cost[i];
			double cost = doneNode->cost + doneNode->edges_cost[i];
			if (nodes[to].cost < 0 || cost < doneNode->edges_cost[i])
			{
				nodes[to].cost = cost;
				nodes[to].toGoal_num = doneNode_num;
			}
		}
	}

	//start����ŒZ�o�H�����ǂ���goal�܂�
	int next = nodes[start].toGoal_num;
	this->CubicInterpolateBVH(&bvhs[((next - 2) % size_interseg) + 2], 5);
	//cout << ((next-2) % size_interseg) + 2 << endl;
	for (int i = 0; i < num_seg - 1; i++) {
		next = nodes[next].toGoal_num;
		this->CubicInterpolateBVH(&bvhs[((next - 2) % size_interseg) + 2], 5);
		//cout << ((next-2) % size_interseg) + 2 << endl;
	}
	this->CubicInterpolateBVH(&bvhs[goal], 5);
}

void ofxDigitalDanceBvh::FillBlank2(int num_seg, ofxDigitalDanceBvh bvhs[], vector<vector<float>>& con_mat, int arraysize, int start, int goal) {

	int size = (arraysize - 2) * num_seg; //�S�m�[�h�̐�-2
	int size_interseg = arraysize - 2; //bvhs�̐�-2
	std::vector<Node> nodes;
	nodes.resize(size + 2);

	//�S�m�[�h�̐ڑ����
	for (int i = 0; i < nodes.size(); i++)
	{
		if (i == start) {
			continue;
		}
		else if (i == goal) {
			nodes[i].edges_to.resize(size_interseg);
			nodes[i].edges_cost.resize(size_interseg);
			for (int j = 0; j < nodes[i].edges_to.size(); j++) {
				nodes[i].edges_to[j] = j + 2;
				nodes[i].edges_cost[j] = 1.0 / bvhs[j + 2].getConnectivity(&bvhs[i]);
				//nodes[i].edges_cost[j] = con_mat[j+2][i];
			}
		}
		else if ((i - 2) / size_interseg == (num_seg - 1)) {
			nodes[i].edges_to.resize(1);
			nodes[i].edges_cost.resize(1);
			nodes[i].edges_to[0] = start;
			nodes[i].edges_cost[0] = 1.0 / bvhs[start].getConnectivity(&bvhs[((i - 2) % size_interseg) + 2]);
			//nodes[i].edges_cost[0] = con_mat[start][((i-2) % size_interseg) + 2];
		}
		else {
			nodes[i].edges_to.resize(size_interseg);
			nodes[i].edges_cost.resize(size_interseg);
			for (int j = 0; j < nodes[i].edges_to.size(); j++) {
				nodes[i].edges_to[j] = ((((i - 2) / size_interseg) + 1) * size_interseg) + j + 2;
				/*double kost0 = bvhs[2].getConnectivity(&bvhs[2] );
				double kost = 1.0 / bvhs[j+2].getConnectivity(&bvhs[((i-2) % size_interseg) + 2 ] );*/
				if (j + 2 == ((i - 2) % size_interseg) + 2) {
					nodes[i].edges_cost[j] = 100.0;
				}
				else {
					nodes[i].edges_cost[j] = con_mat[j + 2][((i - 2) % size_interseg) + 2];
				}
			}
		}
	}

	//�S�m�[�h�̏�����
	for (int i = 0; i < nodes.size(); i++)
	{
		nodes[i].done = false;
		nodes[i].cost = -1.0;
	}

	cout << "�o�H�T����" << endl;
	//goal����T�����܂�
	nodes[goal].cost = 0.0; //�T�����߂̃m�[�h�̃R�X�g��0

							//�A���S���Y�����s
	while (true)
	{
		//�m��m�[�h��T��
		Node* doneNode = NULL; //�m��m�[�h�ɂ����邽�߂�
		int doneNode_num = NULL;
		for (int i = 0; i < nodes.size(); i++)
		{
			if (nodes[i].done || nodes[i].cost < 0)
			{
				continue;
			}
			if (doneNode == NULL || nodes[i].cost < doneNode->cost)
			{
				doneNode = &nodes[i];
				doneNode_num = i;
			}
		}
		//���m��m�[�h���Ȃ��Ȃ邩�AStart���m��m�[�h�ɂȂ�ΏI��
		if (doneNode == NULL || nodes[start].done)
		{
			break;
		}

		//�m��t���O�𗧂Ă�
		doneNode->done = true;  //����ŎQ�Ɛ�̒l����������
								//�ڑ���m�[�h�̏����X�V����
		for (int i = 0; i < doneNode->edges_to.size(); i++)
		{
			int to = doneNode->edges_to[i];
			double cost1 = nodes[1].edges_cost[i];
			double cost2 = doneNode->edges_cost[i];
			double cost = doneNode->cost + doneNode->edges_cost[i];
			if (nodes[to].cost < 0 || cost < doneNode->edges_cost[i])
			{
				nodes[to].cost = cost;
				nodes[to].toGoal_num = doneNode_num;
			}
		}
	}

	//start����ŒZ�o�H�����ǂ���goal�܂�
	int next = nodes[start].toGoal_num;
	this->CubicInterpolateBVH(&bvhs[((next - 2) % size_interseg) + 2], 5);
	//cout << ((next-2) % size_interseg) + 2 << endl;
	for (int i = 0; i < num_seg - 1; i++) {
		next = nodes[next].toGoal_num;
		this->CubicInterpolateBVH(&bvhs[((next - 2) % size_interseg) + 2], 5);
		//cout << ((next-2) % size_interseg) + 2 << endl;
	}
	this->CubicInterpolateBVH(&bvhs[goal], 5);
}

void ofxDigitalDanceBvh::exportFrameBVH(const string &filename, int start, int end, float scale)
{
	out.open(filename.c_str());
	if (!out) {
		std::cerr << "Can't Save BVH" << std::endl;
	}
	string blank;

	// Bone Structure
	DFSJoint(const_cast<ofxBvhJoint*>(this->getJoint(0)), blank, scale);

	// Bone Move
	out << "MOTION" << std::endl;
	out << "Frames: " << this->num_frames << std::endl;
	out << "Frame Time: " << frame_time << std::endl;

	for (int i = start; i<end; i++)
	{
		this->setFrame(i);
		this->update();

		int j;
		for (int j = 0; j< frames.at(i).size(); j++) {
			// Position
			if (j == 0 || j == 1 || j == 2)
			{
				out << frames.at(i).at(j)*scale << " ";
			}
			// Rotation
			else
			{
				out << frames.at(i).at(j) << " ";
			}
		}
		//out << frames.at(i).at(j);
		out << std::endl;
	}

	out.close();

	std::cout << "finish" << std::endl;
}

// Depth First Search for Joint
void ofxDigitalDanceBvh::DFSJoint(ofxBvhJoint *joint, string blank, float scale)
{
	ofVec3f v;
	v = joint->getOffset();

	if (joint->isRoot())
	{
		v.set(ofVec3f(0.0, 0.0, 0.0));
		out << "HIERARCHY" << std::endl;
		out << "ROOT Hips" << "\n" << "{" << std::endl;
		out << " OFFSET " << v.x * scale << " " << v.y * scale << " " << v.z * scale << std::endl;
		out << " CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation" << std::endl;
		for (int i = 0; i<joint->getChildren().size(); i++)
		{
			DFSJoint(joint->getChildren().at(i), blank, scale);
		}
	}
	else if (!joint->isSite())
	{
		blank += " ";
		out << blank.c_str() << "JOINT " << joint->getName() << std::endl;
		out << blank.c_str() << "{" << std::endl;
		out << blank.c_str() << " OFFSET " << v.x * scale << " " << v.y * scale << " " << v.z * scale << std::endl;
		out << blank.c_str() << " CHANNELS " << "3 " << "Zrotation Xrotation Yrotation" << std::endl;
		for (int i = 0; i<joint->getChildren().size(); i++)
		{
			DFSJoint(joint->getChildren().at(i), blank, scale);
		}
	}
	else {
		blank += " ";
		out << blank.c_str() << "End " << joint->getName() << std::endl;
		out << blank.c_str() << "{" << std::endl;
		out << blank.c_str() << " OFFSET " << v.x * scale << " " << v.y * scale << " " << v.z * scale << std::endl;
	}
	out << blank.c_str() << "}" << std::endl;
}

void ofxDigitalDanceBvh::segmentationBVH(string filename, int rows, double data[]) {

	for (int n = 1; n<rows; n++) {
		string file = filename;
		stringstream FileName;
		FileName << file << "-" << setw(3) << setfill('0') << n - 1 << ".bvh";
		FileName >> file;

		string str = " ";
		ofs.open(file);
		// HIERARCHY
		ofs << "HIERARCHY" << endl;
		// ROOT�̏�������
		ofs << "ROOT " << getJoint(0)->getName() << endl;
		ofs << "{" << endl;
		double roff = 0.000000;
		ofs << str << "OFFSET " << roff << " " << roff << " " << roff << endl;
		ofs << str << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation" << endl;
		childroop(getJoint(0), str);
		ofs << "}" << endl;

		//MOTION
		ofs << "MOTION" << endl;
		ofs << "Frames: " << (int)data[n] - (int)data[n - 1] << endl;
		ofs << "Frame Time: " << fixed << setprecision(6) << frame_time << endl;

		for (int i = (int)data[n - 1]; (int)i<data[n]; i++)
		{
			int j = 0;
			for (j = 0; j<frames.at(i).size() - 1; j++) {
				ofs << frames.at(i).at(j) << " ";
			}
			ofs << fixed << frames.at(i).at(j) << std::endl;
		}

		ofs.close();
	}
}

void ofxDigitalDanceBvh::segmentationBVH_4C(string filename) {
	// 4�J�E���g���̃t���[����
	const int new_num_frames = 74;
	// ���Z�O�����g�ɋ�؂邩(�؂�̂�)
	int num_seg = this->num_frames / new_num_frames;

	for (int n = 0; n<num_seg; n++) {
		// file = filename-00i.bvh��
		string file = filename;
		stringstream FileName;
		FileName << file << "-" << setw(3) << setfill('0') << n << ".bvh";
		FileName >> file;

		string str = " ";
		ofs.open("data\\" + filename + "\\" + file);
		// HIERARCHY
		ofs << "HIERARCHY" << endl;
		// ROOT�̏�������
		ofs << "ROOT " << getJoint(0)->getName() << endl;
		ofs << "{" << endl;
		double roff = 0.000000;
		ofs << str << "OFFSET " << roff << " " << roff << " " << roff << endl;
		ofs << str << "CHANNELS 6 Xposition Yposition Zposition Yrotation Xrotation Zrotation" << endl;
		childroop(getJoint(0), str);
		ofs << "}" << endl;

		//MOTION
		ofs << "MOTION" << endl;
		ofs << "Frames: " << new_num_frames << endl;
		ofs << "Frame Time: " << fixed << setprecision(6) << this->frame_time << endl;

		for (int i = n*new_num_frames; i<(n + 1)*new_num_frames; i++)
		{
			int j = 0;
			for (j = 0; j<frames.at(i).size() - 1; j++) {
				ofs << frames.at(i).at(j) << " ";
			}
			ofs << fixed << frames.at(i).at(j) << std::endl;
		}

		ofs.close();
	}
}

void ofxDigitalDanceBvh::drawElipsoid(ofPoint p1, ofPoint p2, float thickness)
{
    ofPushMatrix();
    ofQuaternion q;
    ofVec3f v1(1,0,0);
    ofVec3f v2 = p2 - p1;
    q.makeRotate(v1, v2);
    q.normalize();
    
    float length = (p2-p1).length();
    float angle, x, y, z;
    q.getRotate(angle, x, y, z);

    ofTranslate(p1);
    ofRotate(angle, x, y, z);
    ofScale(1.0, thickness, thickness);
    
    ofDrawSphere(length/2, 0, length/2);
    ofPopMatrix();
}

//
//int ofxDigitalDanceBvh::GreedySampling(float q, float weighteffort[], float max_WE, vector<vector<float>>& D, int Sn[], int n, float rho) {
//	cout << D.size() << endl;
//
//	int bvh_path_num = 0; // �Ԃ�l�̏�����
//	float max_score = 0.0; // �ő剻�������X�R�A
//	
//	for(int i=0; i<D.size(); i++){
//		// ���łɃT���v���Ƃ��đI�΂�Ă���f�[�^�iSn[]�ɂ���f�[�^�j�͏���
//		bool overlapped_data = false;
//		for(int num_Sn = 0; num_Sn < n; num_Sn++){
//			if(i == Sn[num_Sn]){
//				overlapped_data = true;
//			}
//		}
//		if(overlapped_data){
//			continue;
//		}
//		
//		// ���łɑI�΂�Ă��Ȃ���ΕԂ��p�X�ԍ��̒T���Ɉڂ�
//		if(max_score < (rho * rel_Music_WE(q, weighteffort[i], D.size()))+((1.0-rho) * phi_DistanceScore(i, D, Sn, n))){
//			bvh_path_num = i;
//			max_score = (rho * rel_Music_WE(q, weighteffort[i], D.size()))+((1.0-rho) * phi_DistanceScore(i, D, Sn, n));
//		}
//	}
//
//	return bvh_path_num;
//}
//
//float ofxDigitalDanceBvh::rel_Music_WE(float q, float weighteffort, float max_WE){
//	return 1.0 - ((q - weighteffort)/max_WE);
//}
//
//float ofxDigitalDanceBvh::phi_DistanceScore(int bvh_path_num, vector<vector<float>>& D, int Sn[], int n){
//	float min_distance = 1.0;
//	
//	for(int i = 0; i < n; i++){
//		if(D[bvh_path_num][Sn[i]]<min_distance){
//			min_distance = D[bvh_path_num][Sn[i]];
//		}
//	}
//
//	return min_distance;
//}
//
//int ofxDigitalDanceBvh::mat_dim(vector<vector<float>>& con_mat){
//	cout << "������" << con_mat.size() << endl;
//	cout << "��" << con_mat[0].size() << endl;
//	for (int i = 0; i < con_mat.size(); i++)
//	{
//		for (int j = 0; j < con_mat.size(); j++)
//		{
//			cout << con_mat[i][j] << " ";
//		}
//		cout << endl;
//	}
//	return con_mat.size();
//}

int ofxDigitalDanceBvh::GreedySampling(float q, float weighteffort[], float max_WE, vector<vector<float>>& D, int Sn[], int n, float rho) {
	cout << D.size() << endl;

	int bvh_path_num = 0; // �Ԃ�l�̏�����
	float max_score = 0.0; // �ő剻�������X�R�A
	
	for(int i=0; i<D.size(); i++){
		// ���łɃT���v���Ƃ��đI�΂�Ă���f�[�^�iSn[]�ɂ���f�[�^�j�͏���
		bool overlapped_data = false;
		for(int num_Sn = 0; num_Sn < n; num_Sn++){
			if(i == Sn[num_Sn]){
				overlapped_data = true;
			}
		}
		if(overlapped_data){
			continue;
		}
		
		// ���łɑI�΂�Ă��Ȃ���ΕԂ��p�X�ԍ��̒T���Ɉڂ�
		if(max_score < (rho * rel_Music_WE(q, weighteffort[i], D.size()))+((1.0-rho) * phi_DistanceScore(i, D, Sn, n))){
			bvh_path_num = i;
			max_score = (rho * rel_Music_WE(q, weighteffort[i], D.size()))+((1.0-rho) * phi_DistanceScore(i, D, Sn, n));
		}
	}

	return bvh_path_num;
}

float ofxDigitalDanceBvh::rel_Music_WE(float q, float weighteffort, float max_WE){
	return 1.0 - ((q - weighteffort)/max_WE);
}

float ofxDigitalDanceBvh::phi_DistanceScore(int bvh_path_num, vector<vector<float>>& D, int Sn[], int n)
{
	float min_distance = 1.0;
	
	for(int i = 0; i < n; i++){
		if(D[bvh_path_num][Sn[i]]<min_distance){
			min_distance = D[bvh_path_num][Sn[i]];
		}
	}
	return min_distance;
}
