#include <iostream> 
#include <string>
#include <queue>
#include <fstream>
#include <unordered_map>
#include <bitset>
using namespace std;

//Ʈ���� ����� ���� ���
struct Node
{
	char ch;
	int freq;
	Node *left, *right;
};


//��带 �ϳ� ����� ���� �Լ�
Node* makeNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// heap�� ���ؼ� ���Ǵ� �� �Լ�
struct comp
{
	bool operator()(Node* l, Node* r)
	{
		// �󵵼��� ���� ���� ģ�� -> ���� ����
		return l->freq > r->freq;
	}
};


void decode(Node* root, int &index, string str,ofstream &out)
{
 	if (root == nullptr) {
 		return;
	}

 	// node�� ã�´�
 	if (!root->left && !root->right)
 	{
 		out << root->ch;
 		return;
 	}

 	index++;

	//��� �Լ��� ��带 ����
 	if (str[index] == '0')
 		decode(root->left, index, str,out);
	else
 		decode(root->right, index, str,out);
}



// Huffman tree ����� decode, file�� �� ����
void buildHuffmanTree(string text)
{
	//Huffman code ����
	ifstream code;
	code.open("Huffman_code.hbs");

	//decode�� ������ �����ϱ� ���� ����
	ofstream out;
	out.open("Output.txt");

	//stuffing�� ���� ����
	int stuffing = 0; 
	int count = 0;
	char buf[1000000] = { NULL,};
	string str = "";

/////////////////////////Tree �����//////////////////////////////////////

	// ������ character�� ��� �������� count
	unordered_map<char, int> freq;
	for (char ch : text) {
		freq[ch]++;
	}

	// Huffman tree�� node���� �����ϴ� priority queue
	priority_queue<Node*, vector<Node*>, comp> pq;

	// ������ character�� ���ؼ� node �����ϱ� 
	for (auto pair : freq) {
		pq.push(makeNode(pair.first, pair.second, nullptr, nullptr));
	}

	// loop (until notihing in queue)
	while (pq.size() != 1)
	{
		// ���� priority�� ������ -> �󵵼��� ����
		// �󵵼��� ���� �� �ΰ��� queue���� ������
		Node *left = pq.top(); pq.pop();
		Node *right = pq.top();	pq.pop();

		//�� ����� ���� ���� ������ ���� ���� �� ��带 �����ϱ�
		//�� �󵵼��� ���� ���� ��忡 �����ϱ�
		//�ٽ� ��� queue�� �о� �ֱ�
		int sum = left->freq + right->freq;
		pq.push(makeNode('\0', sum, left, right));
	}

	//root -> Huffman Tree�� root
	Node* root = pq.top();

/////////////////////get Huffman code//////////////////////////	
	code.seekg(0, ios::end);

	//tellg() -> get size
	int sz = code.tellg();

	//seekg() -> back pointer to first
	code.seekg(0, ios::beg);

	//binary file -> read
	code.read(buf, sz);

	while (buf[count] != NULL) {
		str += buf[count];
		count++;
	}

	int index = -1;
	while (index < (int)str.size() - 2) {
 		decode(root, index, str,out);
	}

}

int main()
{
	//input file ����
	ifstream in("input.txt");
	string text;
	string all;
	int count = 0;

	if (in.fail()) {
		cout << "failed _ file open" << endl;
		return -1;
	}

	while (!in.eof()) {
		getline(in, text);
		count++;
		all += text;
	}
	buildHuffmanTree(all);

	return 0;
}
