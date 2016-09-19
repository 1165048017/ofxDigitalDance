#pragma once

#include "ofMain.h"
#include "ofxBvh.h"

class ofxDigitalDanceBvh : public ofxBvh
{
public:

	//�t�@�C���̏o��
	void exportBvh(string filename);  //���݂�bvh�t�@�C����filename.bvh�ɓf���o��
	void exportScaledOffset(string filename, float scale); //offset�̑傫���𒲐����ďo��(���ӁI�I�I�I�FXrot,Yrot,Zrot�̏��Ԃ��Ⴄ��)
	vector<vector<double>> exportAngular();

	void exportAngulartoCSV(string filename);  //�e�֐߂̊p�x��csv�t�@�C���ɏo��
	void exportAngulartoTXT(string filename); //txt�t�@�C����
	void exportAngularVelocity(string filename);  //�e�֐߂̊p���x��csv�t�@�C���ɏo��
	float getWeightEffort(); //bvh�t�@�C����WeightEffort��Ԃ��֐�
	float getVelAve(); //bvh�t�@�C���̊e�֐߂̈ʒu���x��Ԃ��֐�

	void childroop(const ofxBvhJoint* joint, string str); //export(filename)�֐��Ŏg��
	void childroopScaledOffset(const ofxBvhJoint* joint, string str, float scale); //exportScaledOffset(string filename, float scale)�֐��Ŏg��

	//���T���v�����O
	void Resampling(const float frametime, string filename); //filename.bvh�Ƀ��T���v�����O����bvh��f���o��

	//���
	void InterpolatePush(string filename, ofxDigitalDanceBvh* next, int range); //�A�����range�t���[�����������`��Ԃŏ�������
	void InterpolateAttitude(string filename, ofxDigitalDanceBvh* next, int range); //�ԉ��т�����
	void LerpBVH(string filename1, string filename2, int range); //�Z�O�����g�Ԃ���`��Ԃ��Čq��
	void LerpBVH(ofxDigitalDanceBvh* next, int range);
	void CubicInterpolateBVH(ofxDigitalDanceBvh* next, int range); //�Z�O�����g�Ԃ��O���֐���Ԃ��Čq��
	float CubicInterpolate(float start, float stop, float amt) {
		return ((-2.0)*(stop - start)*amt*amt*amt) + (3.0*(stop - start)*amt*amt) + start;
	};

	//�A��
	float getConnectivity(ofxDigitalDanceBvh* next); //this.bvh��next.bvh�̘A�����]���֐��̎Z�o
	void FillBlank(int num_seg, ofxDigitalDanceBvh bvhs[], int arraysize, int start = 0, int goal = 1); //�m�[�h�̐ڑ����ݒ莞�ɘA�����̌v�Z�������^�C���Łi�߂�����x���j
	void FillBlank2(int num_seg, ofxDigitalDanceBvh bvhs[], vector<vector<float>>& con_mat, int arraysize, int start = 0, int goal = 1); //�m�[�h�̐ڑ����ݒ莞�́C�A�����̌v�Z�͂قƂ�ǎ��O�v�Z�Łi�_�C�N�X�g���@�j
																															 // ���c����
	void motionInitialize();
	void setTransRotate(ofVec3f root_pos, ofQuaternion *q);
	void exportFrameBVH(const string &filename, int start, int end, float scale);// ��������...
	void DFSJoint(ofxBvhJoint *joint, string blank, float scale);

	// bvh�𕪊�����
	void segmentationBVH(string filename, int rows, double data[]);  //BVH�t�@�C���̕���(�`��)
	void segmentationBVH_4C(string filename); //4�J�E���g���ɕ����iperfume�p�j

	// Sampling��@
	int GreedySampling(float q, float weighteffort[], float max_WE, vector<vector<float>>& D, int Sn[], int n, float rho = 1.0); // Dou����×~�@�ɂ��T���v�����O
	float rel_Music_WE(float q, float weighteffort, float max_WE); // ���y�ƃ_���X�̌������̗ގ��x
	float phi_DistanceScore(int bvh_path_num, vector<vector<float>>& D, int Sn[], int n); // �V���ȃT���v���Ƃ��łɑI�΂ꂽ�T���v���̋����̍ŏ��l��Ԃ��֐�

	int mat_dim(vector<vector<float>>& con_mat);
	
private:
	ofstream ofs;
	ofstream out;	
};


//�m�[�h
struct Node
{
	//���ڐڑ����ꂽ�G�b�W�ƃm�[�h�̏��̏��
	std::vector<int> edges_to; //�ڑ���̃m�[�h�ԍ�
	std::vector<double> edges_cost; //�ڑ����Ă���G�b�W�̃R�X�g

	//�_�C�N�X�g���@�̂��߂̃f�[�^
	bool done; //�m��m�[�h���ۂ�
	double cost; //���̃m�[�h�ւ̌����_�ł̍ŏ��R�X�g�B-1�̂Ƃ��͂��̃m�[�h�𖢒T���Ƃ��Ă��܂��B
	int toGoal_num; ////�S�[���ւ̍ŒZ���[�g�ɂȂ���m�[�h�ԍ�
};