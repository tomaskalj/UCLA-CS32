void removeBad(list<Movie*>& li) {
	list<Movie*>::iterator it = li.begin();
	while (it != li.end()) {
		Movie* movie = *it;
		if (movie->rating() < 55) {
			it = li.erase(it);
			delete movie;
		} else {
			it++;
		}
	}
}