class BTT {
public:

	BTT* left;
	BTT* right;

	BTT() {
		left = 0;
		right = 0;
		bottomN = 0;
		leftN = 0;
		rightN = 0;
	};

	void Split() {
		if (bottomN) {
			if (bottomN->bottomN != this) {
				bottomN->Split();
			}
			Split2();
			bottomN->Split2();
			left->rightN = bottomN->right;
			right->leftN = bottomN->left;
			bottomN->left->rightN = right;
			bottomN->right->leftN = left;

		} else {
			Split2();
			left->rightN = 0;
			right->leftN = 0;
		}
	}

	void Split2() {
		left = new BTT();
		right = new BTT();
		left->leftN = right;
		right->rightN = left;
		left->bottomN = leftN;
		if (leftN) {
			if (leftN->bottomN == this) {
				leftN->bottomN = left;
			} else {
				if (leftN->leftN == this) {
					leftN->leftN == left;
				} else {
					leftN->rightN == left;
				}
			}
		}
		right->bottomN = rightN;
		if (rightN) {
			if (rightN->bottomN = this) {
				rightN->bottomN = right;
			} else {
				if (rightN->rightN == this) {
					rightN->rightN = right;
				} else {
					rightN->leftN = right;
				}
			}
		}
		left->left = 0;
		left->right = 0;
		right->left = 0;
		right->right = 0;
	}

	bool hasChildren() {
		if (left) return true;
		else return false;
	}

private:

	BTT* leftN;
	BTT* rightN;
	BTT* bottomN;

	~BTT(){};
};