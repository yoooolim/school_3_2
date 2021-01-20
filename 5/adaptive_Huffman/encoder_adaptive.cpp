#include <iostream> 
#include <string>
#include <queue>
#include <fstream>
#include <unordered_map>
#include <bitset>
#include <sstream>
using namespace std;

//Ʈ���� ����� ���� ���
struct Node
{
 	char ch;
 	int freq;
 	Node *left, *right;
};

int EncodeFile(string pathname, string outpathname, unordered_map<char, string> tree) {
	ifstream file;
	file.open(pathname);

	string str = "";
	string all = "";

	ofstream outfile;
	outfile.open(outpathname);

	if (file.fail()) {
		cout << pathname << " : failed _ file open" << endl;
		return -1;
	}

	while (!file.eof()) {
		getline(file,str);
		all += str;
	}

	str = "";

	for (char ch : all) {
		str += tree[ch];
	}

	std::stringstream sstream(str);
	while (sstream.good()) {
		bitset<8> bits;
		sstream >> bits;
		char c = char(bits.to_ulong());
		outfile << c;
	}
	outfile.close();
}

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

void encode(Node* root, string str,
 			unordered_map<char, string> &huffmanCode)
 {
 	if (root == nullptr)
 		return;

	// node�� ã�´�
 	if (!root->left && !root->right) {
 		huffmanCode[root->ch] = str;
	}	

	//�������� ���� 0, ���������� ���� 1�� encoding�Ѵ�.
 	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}

unordered_map<char, string> buildHuffmanTree_all(string text)
{
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

	/////////////////////ENCODING/////////////////////////	
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	return huffmanCode;
}


// Huffman tree ����� decode, file�� �� ����
unordered_map<char, string> buildHuffmanTree(string text, int alp)
{
/////////////////////////Tree �����//////////////////////////////////////

	// ������ character�� ��� �������� count
 	unordered_map<char, int> freq;

	int location = 0;
	while (true) {
		location = (int)(text.find((char)(alp), location+1));
		if (location == -1) break;
		freq[toupper((char)(text.at(location+1)))]++;
	}
	while (true) {
		location = (int)(text.find((char)(alp+32), location+1));
		if (location == -1) break;
		freq[toupper((char)(text.at(location + 1)))]++;
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

	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);
	
	ofstream file;
	file.open("context_adaptive_huffman_table.hbs", std::ios::out | std::ios::app);
	file << bitset<8>(alp);
	for (auto pair : huffmanCode) {
		file << bitset<8>(pair.first) << bitset<8>(pair.second.length()) << pair.second;
	}
	file.close();
	
	cout << "========================================================================" << endl;
	cout << (char)alp << "'s adaptive encoding table" << endl << endl;
	for (auto pair : huffmanCode) {
		cout << pair.first << "	" << pair.second << endl;
	}
	cout << "========================================================================" << endl << endl;

	return huffmanCode;
}


int main()
{
	ifstream in("training_input.txt");
	string text;
	string all;
	int count=0;

	unordered_map<char, string> HTALL,HTA,HTB,HTC,HTD,HTE,HTF,HTG,HTH,HTI,HTJ,HTK,HTL,HTM,HTN,HTO,HTP,HTQ,HTR,HTS,HTT,HTU,HTV,HTW,HTX,HTY,HTZ;

	if (in.fail()) {
		cout << "failed _ file open" << endl;
		return -1;
	}

	while (!in.eof()) {
		getline(in, text);
		count++;
		all += text;
	}

	HTALL = buildHuffmanTree_all(all);
 	HTA = buildHuffmanTree(all, 65);
	HTB = buildHuffmanTree(all, 66);
	HTC = buildHuffmanTree(all, 67);
	HTD = buildHuffmanTree(all, 68);
	HTE = buildHuffmanTree(all, 69);
	HTF = buildHuffmanTree(all, 70);
	HTG = buildHuffmanTree(all, 71);
	HTH = buildHuffmanTree(all, 72);
	HTI = buildHuffmanTree(all, 73);
	HTJ = buildHuffmanTree(all, 74);
	HTK = buildHuffmanTree(all, 75);
	HTL = buildHuffmanTree(all, 76);
	HTM = buildHuffmanTree(all, 77);
	HTN = buildHuffmanTree(all, 78);
	HTO = buildHuffmanTree(all, 79);
	HTP = buildHuffmanTree(all, 80);
	HTQ = buildHuffmanTree(all, 81);
	HTR = buildHuffmanTree(all, 82);
	HTS = buildHuffmanTree(all, 83);
	HTT = buildHuffmanTree(all, 84);
	HTU = buildHuffmanTree(all, 85);
	HTV = buildHuffmanTree(all, 86);
	HTW = buildHuffmanTree(all, 87);
	HTX = buildHuffmanTree(all, 88);
	HTY = buildHuffmanTree(all, 89);
	HTZ = buildHuffmanTree(all, 90);
	
	EncodeFile("test_input1.txt", "test_input1_code.hbs", HTALL);
	EncodeFile("test_input2.txt", "test_input2_code.hbs", HTALL);
	EncodeFile("test_input3.ttt", "test_input3_code.hbs", HTALL);

 	return 0;
}
