void listAll(const File* f, string path)  { // two-parameter overload
    if (f != nullptr) {
        string newPath = path + "/" + f->name();
        cout << newPath << endl;

        if (f->files() != nullptr) {
            for (int i = 0; i < f->files()->size(); i++) {
                const File* subFile = f->files()->at(i);
                listAll(subFile, newPath);
            }
        }
    }
}