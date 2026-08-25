#if !defined(HEURISTIC)
#define HEURISTIC
namespace Heuristic
{
    constexpr int CO_SIZE    = 2187;   // 3^7
    constexpr int EO_SIZE    = 2048;   // 2^11
    constexpr int SLICE_SIZE = 495;    // C(12,4)
    
    constexpr int TWIST_SLICE_SIZE = CO_SIZE * SLICE_SIZE;
    constexpr int FLIP_SLICE_SIZE  = EO_SIZE * SLICE_SIZE;
    
    constexpr uint8_t UNVISITED = 255;
    
    
    // Phase 1 pruning tables
    
    extern std::vector<uint8_t> twistSliceTable;
    
    extern std::vector<uint8_t> flipSliceTable;
    
    
    // Phase 2 coordinate sizes
    
    constexpr int CP_SIZE         = 40320; // 8!
    constexpr int UD_EDGE_SIZE    = 40320; // 8!
    constexpr int SLICE_PERM_SIZE = 24;    // 4!
    
    
    // Phase 2 coordinate sizes
    
    constexpr int CP_SLICE_SIZE = CP_SIZE * SLICE_PERM_SIZE;
    
    constexpr int UD_EDGE_SLICE_SIZE = UD_EDGE_SIZE * SLICE_PERM_SIZE;
    
    
    // Phase 2 pruning tables
    
    extern std::vector<uint8_t> cpSliceTable;
    
    extern std::vector<uint8_t> udEdgeSliceTable;
}
class Heuristictable
{
    public:
        void generateTables();
        void loadTables();
        void loadTable(const std::string& filename, std::vector<uint8_t>& table);
        void saveTable(const std::string& filename,const std::vector<uint8_t>& table);
        void load_or_generatetable();
};
#endif // MACRO