#include "../src/node.h"
#include "../src/folder.h"
#include "../src/file.h"
#include "../src/visitor.h"
#include "../src/find_by_name_visitor.h"
#include "../src/stream_out_visitor.h"
#include "../src/tree_visitor.h"
#include "../src/order_by.h"
#include "../src/file_system_parser.h"
#include "../src/file_system_builder.h"

class VisitorTest: public ::testing::Test {
protected:
    virtual void SetUp() {
        home = new Folder("structure/home");

        profile = new File("structure/home/my_profile");
        home->add(profile);

        hello1 = new File("structure/home/hello.txt");
        home->add(hello1);

        document = new Folder("structure/home/Documents");
        home->add(document);

        favorite = new Folder("structure/home/Documents/favorites");
        document->add(favorite);
        ddd = new File("structure/home/Documents/favorites/domain-driven-design.pdf");
        favorite->add(ddd);
        ca = new File("structure/home/Documents/favorites/clean-architecture.pdf");
        favorite->add(ca);
        cqrs = new File("structure/home/Documents/favorites/cqrs.pdf");
        favorite->add(cqrs);

        note = new File("structure/home/Documents/note.txt");
        document->add(note);

        hello2 = new File("structure/home/hello.txt");
        home->add(hello2);

        download = new Folder("structure/home/Downloads");
        home->add(download);

        funny = new File("structure/home/Downloads/funny.png");
        download->add(funny);

        visitor_folder = new Folder("structure/visitor");
        file1 = new File("structure/visitor/file1.txt");
        visitor_folder->add(file1);
        file2 = new File("structure/visitor/file2.txt");
        visitor_folder->add(file2);
        nested = new Folder("structure/visitor/nested");
        visitor_folder->add(nested);
        file3 = new File("structure/visitor/nested/file3.txt");
        nested->add(file3);
        file4 = new File("structure/visitor/nested/file4.txt");
        nested->add(file4);
    }

    void TearDown() {
        delete home;
        delete profile;
        delete download;
        delete document;
        delete note;
        delete favorite;
        delete ddd;
        delete ca;
        delete cqrs;
        delete funny;
        delete hello1;
        delete hello2;
        delete visitor_folder;
        delete file1;
        delete file2;
        delete nested;
        delete file3;
        delete file4;
    }
    
    Node * home;
    Node * profile;
    Node * download;
    Node * document;
    Node * note;
    Node * favorite;
    Node * ddd;
    Node * ca;
    Node * cqrs;
    Node * funny;
    Node * hello1;
    Node * hello2;

    Node * visitor_folder;
    Node * file1;
    Node * file2;
    Node * nested;
    Node * file3;
    Node * file4;
};

TEST_F(VisitorTest, findNormal) {
    FindByNameVisitor * visitor = new FindByNameVisitor("clean-architecture.pdf");

    home->accept(visitor);

    ASSERT_EQ(1, visitor->getPaths().size());
    ASSERT_EQ(ca->path(), visitor->getPaths().begin().operator*());
}

TEST_F(VisitorTest, findMany) {
    FindByNameVisitor * visitor = new FindByNameVisitor("hello.txt");

    home->accept(visitor);

    ASSERT_EQ(2, visitor->getPaths().size());
}

TEST_F(VisitorTest, findNothing) {
    FindByNameVisitor * visitor = new FindByNameVisitor("nothing-to_find");

    home->accept(visitor);

    ASSERT_EQ(0, visitor->getPaths().size());
}

TEST_F(VisitorTest, streamOutFile) {
    StreamOutVisitor * visitor = new StreamOutVisitor();

    profile->accept(visitor);

    string expected;
    expected += "_____________________________________________\n";
    expected += "structure/home/my_profile\n";
    expected += "---------------------------------------------\n";
    expected += "Profile\n";
    expected += "Name: name\n";
    expected += "_____________________________________________\n";

    ASSERT_EQ(expected, visitor->getResult());
}

TEST_F(VisitorTest, streamOutFolder) {
    StreamOutVisitor * visitor = new StreamOutVisitor();

    nested->accept(visitor);

    string expected;
    expected += "_____________________________________________\n";
    expected += "structure/visitor/nested/file3.txt\n";
    expected += "---------------------------------------------\n";
    expected += "I am file 3\n";
    expected += "_____________________________________________\n";
    expected += "\n";
    expected += "_____________________________________________\n";
    expected += "structure/visitor/nested/file4.txt\n";
    expected += "---------------------------------------------\n";
    expected += "I am file 4\n";
    expected += "_____________________________________________\n";
    expected += "\n";

    ASSERT_EQ(expected, visitor->getResult());
}

TEST_F(VisitorTest, streamOutNestedFolder) {
    StreamOutVisitor * visitor = new StreamOutVisitor();

    visitor_folder->accept(visitor);

    string expected;
    expected += "_____________________________________________\n";
    expected += "structure/visitor/file1.txt\n";
    expected += "---------------------------------------------\n";
    expected += "I am file 1\n";
    expected += "_____________________________________________\n";
    expected += "\n";
    expected += "_____________________________________________\n";
    expected += "structure/visitor/file2.txt\n";
    expected += "---------------------------------------------\n";
    expected += "I am file 2\n";
    expected += "_____________________________________________\n";
    expected += "\n";
    expected += "_____________________________________________\n";
    expected += "structure/visitor/nested/file3.txt\n";
    expected += "---------------------------------------------\n";
    expected += "I am file 3\n";
    expected += "_____________________________________________\n";
    expected += "\n";
    expected += "_____________________________________________\n";
    expected += "structure/visitor/nested/file4.txt\n";
    expected += "---------------------------------------------\n";
    expected += "I am file 4\n";
    expected += "_____________________________________________\n";
    expected += "\n";

    ASSERT_EQ(expected, visitor->getResult());
}

TEST_F(VisitorTest, TreeVisitorByNameIterator) {
    FileSystemBuilder * builder = new FileSystemBuilder();
    FileSystemParser * parser = new FileSystemParser(builder);
    parser->setPath("structure");
    parser->parse();
    Folder * root = parser->getRoot();
    TreeVisitor * visitor = new TreeVisitor(OrderBy::Name);
    root->accept(visitor);

    string expecedResult = ".\n"
                           "├── file.txt\n"
                           "├── home\n"
                           "│   ├── Documents\n"
                           "│   │   ├── favorites\n"
                           "│   │   │   ├── clean-architecture.pdf\n"
                           "│   │   │   ├── cqrs.pdf\n"
                           "│   │   │   └── domain-driven-design.pdf\n"
                           "│   │   ├── hello.txt\n"
                           "│   │   └── note.txt\n"
                           "│   ├── Downloads\n"
                           "│   │   └── funny.png\n"
                           "│   ├── hello.txt\n"
                           "│   └── my_profile\n"
                           "└── visitor\n"
                           "    ├── file1.txt\n"
                           "    ├── file2.txt\n"
                           "    └── nested\n"
                           "        ├── file3.txt\n"
                           "        ├── file4.txt\n"
                           "        └── nested2\n"
                           "            ├── file5.txt\n"
                           "            └── file6.txt\n";
    ASSERT_EQ(expecedResult, visitor->getTree());
}