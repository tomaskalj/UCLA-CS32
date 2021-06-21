void removeOdds(vector<int>& v) {
	for (int i = v.size() - 1; i >= 0; i--) {
		if (v.at(i) % 2 == 1) {
			v.erase(v.begin() + i);
		}
	}
}