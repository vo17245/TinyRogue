#include "CreateAtlas.h"
int Dispatch(int argc, char** argv)
{
    if (argc < 2)
    {
        std::print("Usage: %s create_atlas <output_path> <duration> <sequence_path1> <sequence_path2> ...\n", argv[0]);
        return 1;
    }
    std::string action = argv[1];
    if (action == "create_atlas")
    {
        return HandleCreateAtlasAction(argc, argv);
    }
}
int main(int argc, char** argv)
{
    return Dispatch(argc, argv);
}