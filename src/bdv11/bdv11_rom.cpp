#include "types.h"
#include "bdv11.h"

using std::array;

// The following array is the contents of the 23-045E2 and 23-046E2 ROMs.
//
const array<u16, 2048> BDV11::ROM_045E2_046E2 =
{
	0112737, 0000016, 0177524, 0000005, 0012700, 0000340, 0106400, 0012706, 
	0177524, 0012737, 0000400, 0177520, 0005001, 0005002, 0005003, 0005004, 
	0005005, 0004067, 0000004, 0000000, 0000753, 0000257, 0001774, 0100773, 
	0102772, 0103771, 0002770, 0003767, 0101766, 0101015, 0100364, 0002363, 
	0003362, 0002400, 0000277, 0001357, 0100356, 0102355, 0103354, 0003353, 
	0101352, 0002751, 0002002, 0000270, 0000761, 0022700, 0173046, 0001343, 
	0022706, 0177522, 0001340, 0022737, 0000340, 0177522, 0001334, 0005726, 
	0022706, 0177524, 0001330, 0000277, 0005001, 0001041, 0101040, 0003037, 
	0005101, 0103035, 0003034, 0005401, 0101032, 0002431, 0006001, 0103027, 
	0100026, 0005501, 0103424, 0000261, 0100022, 0002021, 0006101, 0103017, 
	0102016, 0006201, 0103014, 0102013, 0005601, 0003011, 0006301, 0002407, 
	0005201, 0001405, 0005301, 0001003, 0005701, 0003001, 0001402, 0004067, 
	0177544, 0112702, 0000377, 0001441, 0105002, 0101037, 0105102, 0103035, 
	0100034, 0105602, 0103432, 0105202, 0100030, 0105402, 0101026, 0105302, 
	0103024, 0003023, 0000302, 0001421, 0106302, 0002017, 0106002, 0103415, 
	0003414, 0106202, 0103012, 0102411, 0106102, 0100007, 0001406, 0105502, 
	0100404, 0001003, 0105702, 0103401, 0001402, 0004067, 0177430, 0012701, 
	0000001, 0012702, 0173416, 0012737, 0020000, 0177522, 0012703, 0173502, 
	0030137, 0177522, 0001410, 0012205, 0012204, 0022305, 0001011, 0021304, 
	0001007, 0000167, 0000016, 0022222, 0006301, 0022701, 0040000, 0001360, 
	0004067, 0177342, 0012700, 0000001, 0012701, 0173662, 0030037, 0177524, 
	0001401, 0000445, 0022121, 0006300, 0022700, 0000020, 0001367, 0005000, 
	0005010, 0013737, 0177524, 0000002, 0052737, 0100000, 0000002, 0013703, 
	0000002, 0042703, 0177017, 0012702, 0000005, 0005737, 0000002, 0100403, 
	0022103, 0001414, 0000403, 0005721, 0021103, 0001410, 0005721, 0022121, 
	0077214, 0032737, 0007000, 0177524, 0001406, 0000401, 0005721, 0012105, 
	0012104, 0000167, 0000120, 0005737, 0000002, 0100001, 0000000, 0000167, 
	0177116, 0173470, 0001402, 0173634, 0007416, 0173000, 0006414, 0173302, 
	0003406, 0054104, 0000040, 0173000, 0005412, 0046104, 0000100, 0173034, 
	0004410, 0045504, 0000200, 0173536, 0004410, 0054504, 0000140, 0173050, 
	0005412, 0000000, 0000400, 0173050, 0000400, 0173000, 0007416, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0017042, 
	0010001, 0010111, 0005721, 0020103, 0101774, 0024101, 0001402, 0011104, 
	0000000, 0020100, 0001372, 0005002, 0010221, 0020103, 0101775, 0005102, 
	0010241, 0020211, 0001402, 0011104, 0000000, 0020100, 0001371, 0010301, 
	0005702, 0001365, 0012702, 0125125, 0110241, 0000302, 0110241, 0020211, 
	0001402, 0011104, 0000000, 0000302, 0162701, 0002000, 0103365, 0000400, 
	0112737, 0000016, 0177524, 0012701, 0173432, 0010046, 0012100, 0005040, 
	0010002, 0005740, 0010210, 0005700, 0003372, 0012600, 0012106, 0012104, 
	0012102, 0012103, 0004767, 0000200, 0104777, 0004767, 0000132, 0004767, 
	0000166, 0104377, 0004767, 0000120, 0024444, 0004767, 0000152, 0000004, 
	0004767, 0000104, 0004767, 0000140, 0000003, 0004767, 0000072, 0004767, 
	0000126, 0107777, 0004767, 0000060, 0004767, 0000114, 0000101, 0004767, 
	0000046, 0000450, 0106705, 0020627, 0001074, 0001014, 0022316, 0001012, 
	0022766, 0000340, 0000002, 0001006, 0120521, 0001004, 0005705, 0103401, 
	0000006, 0000002, 0004067, 0000354, 0106705, 0120527, 0000340, 0001012, 
	0020627, 0001076, 0001007, 0005044, 0010046, 0010400, 0005744, 0010014, 
	0012600, 0000207, 0004067, 0000314, 0111164, 0177776, 0011264, 0177774, 
	0000257, 0000207, 0012706, 0177524, 0012700, 0000004, 0012701, 0173672, 
	0012704, 0000400, 0012705, 0173514, 0000556, 0000400, 0001100, 0000040, 
	0173452, 0173454, 0100601, 0102210, 0100602, 0173264, 0173172, 0173204, 
	0173220, 0173232, 0173244, 0173256, 0012701, 0173672, 0022701, 0173672, 
	0001014, 0013702, 0173672, 0016703, 0000160, 0021102, 0001006, 0021103, 
	0001004, 0017704, 0000144, 0021204, 0001402, 0004067, 0000146, 0012700, 
	0173672, 0012001, 0012002, 0022700, 0173676, 0001045, 0011203, 0022703, 
	0052525, 0001041, 0012204, 0020403, 0001036, 0012204, 0022704, 0125252, 
	0001032, 0014205, 0020405, 0001027, 0014205, 0020503, 0001024, 0015001, 
	0020103, 0001021, 0015001, 0020401, 0001016, 0013002, 0020204, 0001013, 
	0013002, 0020302, 0001010, 0016002, 0000004, 0020102, 0001004, 0017002, 
	0177776, 0020302, 0001415, 0004067, 0000012, 0173676, 0173700, 0125252, 
	0052525, 0125252, 0000000, 0012705, 0173000, 0012704, 0000400, 0000423, 
	0012705, 0173000, 0012704, 0002404, 0000416, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0020656, 
	0012700, 0173564, 0012001, 0012002, 0012003, 0011304, 0010305, 0011172, 
	0000010, 0022170, 0000020, 0001040, 0011166, 0177776, 0022166, 0177776, 
	0001033, 0011133, 0022135, 0001030, 0020305, 0001026, 0022705, 0173614, 
	0001023, 0011153, 0022155, 0001020, 0020102, 0001016, 0010405, 0011114, 
	0022115, 0001012, 0011124, 0022125, 0001007, 0020405, 0001005, 0020604, 
	0001003, 0011145, 0022144, 0001402, 0004067, 0000476, 0012702, 0173566, 
	0012203, 0012204, 0011405, 0010637, 0177522, 0020615, 0001011, 0011267, 
	0004340, 0021215, 0001005, 0012777, 0052525, 0000416, 0021315, 0001402, 
	0004067, 0000426, 0012702, 0173570, 0011203, 0011304, 0005014, 0005124, 
	0005444, 0006033, 0005553, 0000261, 0006172, 0000022, 0006266, 0177776, 
	0005614, 0006324, 0005244, 0005333, 0005753, 0001402, 0004067, 0000352, 
	0012714, 0177777, 0105014, 0105124, 0105644, 0105233, 0105453, 0105372, 
	0000022, 0000366, 0177776, 0106314, 0106024, 0106244, 0106133, 0105553, 
	0105724, 0001002, 0105724, 0001402, 0004067, 0000276, 0012701, 0173570, 
	0011104, 0011403, 0014102, 0014105, 0005013, 0113134, 0121513, 0001021, 
	0111305, 0113113, 0121254, 0001015, 0011302, 0022705, 0177652, 0001011, 
	0022702, 0000125, 0001006, 0113104, 0120304, 0001003, 0112103, 0120503, 
	0001402, 0004067, 0000204, 0012700, 0173564, 0012001, 0012002, 0012003, 
	0011304, 0012700, 0177777, 0000277, 0050005, 0100042, 0041205, 0032205, 
	0001037, 0050033, 0100035, 0042153, 0100433, 0031214, 0001031, 0022137, 
	0177522, 0001026, 0022205, 0001024, 0150005, 0100022, 0144205, 0100420, 
	0131205, 0001016, 0000305, 0100014, 0130514, 0001012, 0150514, 0100010, 
	0000314, 0100406, 0131233, 0001004, 0144153, 0001002, 0021214, 0001017, 
	0004067, 0000046, 0173572, 0173602, 0173612, 0125252, 0052525, 0177400, 
	0000377, 0052525, 0125252, 0000377, 0177400, 0177522, 0173612, 0012705, 
	0173000, 0012704, 0003406, 0000167, 0000134, 0000000, 0012705, 0173000, 
	0012704, 0000400, 0000450, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0065162, 
	0012701, 0173234, 0011104, 0014103, 0061104, 0101425, 0003424, 0061104, 
	0101422, 0002421, 0061104, 0003417, 0010451, 0011104, 0064114, 0101413, 
	0002012, 0064114, 0101010, 0002007, 0061114, 0101005, 0002004, 0064114, 
	0002402, 0003001, 0001402, 0004067, 0000144, 0012701, 0173234, 0014104, 
	0161104, 0002427, 0003026, 0103425, 0161104, 0003023, 0101022, 0161104, 
	0101420, 0002017, 0010451, 0011104, 0164114, 0003413, 0101012, 0161114, 
	0103410, 0102407, 0005741, 0164114, 0103004, 0003403, 0022714, 0077777, 
	0001402, 0004067, 0000050, 0012701, 0000002, 0010102, 0005302, 0002407, 
	0005701, 0002405, 0077105, 0005701, 0001002, 0000167, 0000040, 0004067, 
	0000014, 0100003, 0152525, 0125252, 0177522, 0052525, 0025252, 0000000, 
	0005726, 0012704, 0000400, 0012705, 0173000, 0000167, 0000510, 0012700, 
	0000002, 0012701, 0173666, 0012704, 0000400, 0012705, 0173514, 0000167, 
	0000464, 0112737, 0000014, 0177524, 0012705, 0173524, 0012704, 0007416, 
	0000167, 0000442, 0013703, 0177562, 0012704, 0173672, 0112402, 0004767, 
	0000302, 0105714, 0001373, 0010102, 0004767, 0000270, 0000302, 0004767, 
	0000262, 0012704, 0173677, 0112402, 0004767, 0000250, 0105714, 0001373, 
	0112737, 0000013, 0177524, 0005000, 0005003, 0005004, 0005037, 0000000, 
	0004767, 0000044, 0010403, 0000304, 0001773, 0010403, 0005004, 0004767, 
	0000026, 0162704, 0000070, 0062704, 0000010, 0103005, 0110400, 0010037, 
	0000000, 0004767, 0000002, 0000705, 0105737, 0177560, 0100375, 0105004, 
	0153704, 0177562, 0142704, 0000200, 0122704, 0000015, 0001406, 0122704, 
	0000040, 0103267, 0122704, 0000177, 0101664, 0110402, 0004767, 0000104, 
	0122704, 0000015, 0001401, 0000207, 0112702, 0000012, 0004767, 0000064, 
	0042703, 0020040, 0005703, 0001001, 0000644, 0022703, 0000116, 0001420, 
	0022703, 0000131, 0001003, 0012705, 0173536, 0000404, 0010337, 0000002, 
	0012705, 0173566, 0012701, 0173702, 0012704, 0000400, 0000167, 0000126, 
	0000000, 0000617, 0005005, 0105737, 0177564, 0100003, 0110237, 0177566, 
	0000207, 0005205, 0001370, 0000000, 0000604, 0006440, 0020012, 0006400, 
	0051412, 0040524, 0052122, 0020077, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0161744, 
	0112737, 0000004, 0177524, 0012737, 0004410, 0177520, 0106437, 0000026, 
	0013705, 0000024, 0012704, 0000400, 0000167, 0000732, 0042700, 0000004, 
	0005037, 0000000, 0012701, 0174400, 0012706, 0001100, 0004767, 0000040, 
	0004767, 0000052, 0103003, 0077204, 0004767, 0000426, 0012702, 0001100, 
	0004767, 0000056, 0004767, 0000126, 0004767, 0000172, 0004767, 0000272, 
	0112737, 0000012, 0177524, 0112702, 0000010, 0010004, 0000304, 0042704, 
	0000016, 0052704, 0000004, 0012761, 0000013, 0000004, 0004767, 0000274, 
	0000207, 0042704, 0000016, 0052704, 0000010, 0004767, 0000256, 0103014, 
	0032703, 0040000, 0001002, 0010305, 0000402, 0004767, 0000140, 0004767, 
	0177714, 0077221, 0004767, 0000272, 0016105, 0000006, 0000207, 0012702, 
	0000010, 0042704, 0000016, 0052704, 0000006, 0042705, 0100177, 0005205, 
	0010561, 0000004, 0004767, 0000164, 0103005, 0004767, 0177664, 0077217, 
	0004767, 0000216, 0000207, 0012702, 0000010, 0042704, 0000016, 0052704, 
	0000014, 0005061, 0000002, 0005061, 0000004, 0012761, 0177400, 0000006, 
	0004767, 0000110, 0103003, 0077217, 0004767, 0000146, 0000207, 0010405, 
	0042705, 0000016, 0052705, 0000004, 0012761, 0000003, 0000004, 0010511, 
	0004767, 0000052, 0016105, 0000006, 0000207, 0112737, 0000011, 0177524, 
	0022737, 0000240, 0000000, 0001402, 0004767, 0000066, 0112737, 0000037, 
	0177524, 0005005, 0012704, 0000400, 0000167, 0000312, 0010411, 0005003, 
	0012746, 0000100, 0105711, 0100407, 0005203, 0001374, 0005316, 0001372, 
	0011103, 0004767, 0000014, 0005726, 0005711, 0100001, 0000261, 0011103, 
	0000207, 0004767, 0177630, 0011602, 0000000, 0000167, 0177316, 0112737, 
	0000012, 0177524, 0012703, 0000010, 0012701, 0177404, 0010004, 0005002, 
	0052704, 0000010, 0006304, 0103376, 0010461, 0000006, 0032761, 0000200, 
	0177774, 0001774, 0012761, 0177000, 0000002, 0012711, 0000005, 0105711, 
	0100404, 0005202, 0005702, 0001373, 0000404, 0005711, 0100402, 0000167, 
	0177572, 0005303, 0001341, 0011103, 0016102, 0177776, 0000000, 0000727, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0124453, 
	0012706, 0003100, 0012737, 0177777, 0003110, 0004767, 0000306, 0052704, 
	0000006, 0004767, 0000366, 0052704, 0000002, 0005005, 0004767, 0000424, 
	0103772, 0004767, 0000572, 0000761, 0012706, 0003100, 0005037, 0003110, 
	0004767, 0000240, 0052704, 0000400, 0012702, 0000010, 0012737, 0000001, 
	0003102, 0005037, 0003106, 0052704, 0000006, 0004767, 0000432, 0103003, 
	0077213, 0004767, 0000554, 0012737, 0000100, 0003104, 0032704, 0000400, 
	0001403, 0012737, 0000200, 0003104, 0013761, 0003102, 0000002, 0004767, 
	0000370, 0103756, 0012761, 0000001, 0000002, 0004767, 0000354, 0103011, 
	0032704, 0000400, 0001003, 0052704, 0000400, 0000742, 0042704, 0000400, 
	0000737, 0042704, 0000004, 0004767, 0000316, 0103732, 0013761, 0003104, 
	0000002, 0004767, 0000304, 0103724, 0013761, 0003106, 0000002, 0004767, 
	0000270, 0103716, 0063737, 0003104, 0003106, 0063737, 0003104, 0003106, 
	0062737, 0000002, 0003102, 0022737, 0000011, 0003102, 0001274, 0004767, 
	0000316, 0000656, 0005037, 0000000, 0012701, 0177170, 0112737, 0000012, 
	0177524, 0012704, 0000001, 0005700, 0001404, 0052704, 0000020, 0012700, 
	0000001, 0012702, 0000740, 0004767, 0000212, 0000241, 0032761, 0000200, 
	0000002, 0001003, 0077210, 0004767, 0000270, 0000207, 0012702, 0000010, 
	0004767, 0000124, 0103003, 0077204, 0004767, 0000246, 0032711, 0000040, 
	0001006, 0112761, 0000001, 0000002, 0004767, 0000076, 0000763, 0042704, 
	0000016, 0000207, 0012702, 0000010, 0004767, 0000054, 0103003, 0077204, 
	0004767, 0000176, 0032704, 0000002, 0001003, 0112561, 0000002, 0000402, 
	0116125, 0000002, 0004767, 0000022, 0103001, 0000756, 0032711, 0000040, 
	0001761, 0042704, 0000016, 0000207, 0010411, 0005003, 0105711, 0100410, 
	0032711, 0000040, 0001005, 0005203, 0001371, 0011103, 0004767, 0000102, 
	0005711, 0100014, 0010405, 0042705, 0000016, 0052705, 0000012, 0010511, 
	0032711, 0000040, 0001775, 0016105, 0000002, 0000261, 0011103, 0000207, 
	0112737, 0000011, 0177524, 0022737, 0000240, 0000000, 0001402, 0004767, 
	0000020, 0112737, 0000037, 0177524, 0005005, 0012704, 0000400, 0000167, 
	0000064, 0004767, 0177676, 0011602, 0000000, 0005737, 0003110, 0001402, 
	0000167, 0177066, 0000167, 0177130, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0113667, 
	0112737, 0000012, 0177524, 0012701, 0000010, 0012702, 0173042, 0030137, 
	0177524, 0001014, 0005722, 0006301, 0032701, 0000100, 0001770, 0000000, 
	0000757, 0010003, 0000000, 0167004, 0122243, 0000002, 0005000, 0011201, 
	0000400, 0012706, 0017776, 0012705, 0000006, 0012715, 0000340, 0012745, 
	0173050, 0012702, 0173734, 0010146, 0012701, 0175610, 0111603, 0001007, 
	0032737, 0000100, 0177524, 0001414, 0012701, 0176500, 0000411, 0012701, 
	0160010, 0005711, 0111204, 0060401, 0005201, 0040401, 0005703, 0001371, 
	0160515, 0006300, 0006300, 0006300, 0060001, 0000316, 0112600, 0012706, 
	0017460, 0012704, 0173672, 0012737, 0000012, 0177524, 0060004, 0112403, 
	0012711, 0000006, 0012761, 0036226, 0000002, 0105700, 0001407, 0100003, 
	0012761, 0101226, 0000002, 0032711, 0001000, 0001775, 0032737, 0000100, 
	0177524, 0001003, 0032711, 0020000, 0001775, 0022121, 0052721, 0000030, 
	0012711, 0000626, 0000401, 0112411, 0105761, 0177776, 0100375, 0005303, 
	0001372, 0112737, 0000010, 0177524, 0042741, 0000020, 0024141, 0005004, 
	0012703, 0000010, 0004767, 0000062, 0001312, 0122527, 0000220, 0001307, 
	0113703, 0000002, 0042703, 0177700, 0000303, 0151503, 0122323, 0005004, 
	0112737, 0000011, 0177524, 0004767, 0000032, 0001271, 0005715, 0001267, 
	0112737, 0000037, 0177524, 0000137, 0000006, 0042711, 0000024, 0012711, 
	0000422, 0012746, 0005046, 0005005, 0012702, 0000017, 0005046, 0105711, 
	0100416, 0005316, 0001374, 0005302, 0001372, 0105706, 0003242, 0005011, 
	0012703, 0000012, 0005302, 0001376, 0005303, 0001374, 0000630, 0112737, 
	0000007, 0177524, 0005726, 0042711, 0000400, 0116114, 0000002, 0105700, 
	0001013, 0005716, 0001411, 0121427, 0000220, 0001406, 0121427, 0000005, 
	0001403, 0121427, 0000201, 0001330, 0112737, 0000006, 0177524, 0005016, 
	0112446, 0012702, 0000010, 0000241, 0006005, 0006016, 0102006, 0012746, 
	0120001, 0040516, 0042705, 0120001, 0052605, 0005302, 0003364, 0005726, 
	0005303, 0003302, 0010526, 0000207, 0113024, 0113226, 0002220, 0000300, 
	0000400, 0050021, 0005010, 0000001, 0171043, 0177422, 0002220, 0000300, 
	0000400, 0050021, 0002010, 0000001, 0030502, 0113024, 0113226, 0002220, 
	0000300, 0000400, 0050021, 0001010, 0000001, 0030242, 0007407, 0003407, 
	0000007, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0056040, 
	0000167, 0000432, 0012701, 0173516, 0012700, 0001000, 0012716, 0000200, 
	0030012, 0001003, 0005721, 0006300, 0000773, 0012766, 0000020, 0000002, 
	0011166, 0000004, 0111137, 0177521, 0012704, 0173400, 0066666, 0000002, 
	0000004, 0005000, 0004767, 0000120, 0105303, 0001373, 0004767, 0000110, 
	0004767, 0000104, 0010305, 0004767, 0000076, 0000303, 0050305, 0010502, 
	0024242, 0004767, 0000062, 0010305, 0004767, 0000054, 0000303, 0050305, 
	0005702, 0001010, 0004767, 0000040, 0113701, 0177521, 0012737, 0007416, 
	0177520, 0000476, 0010501, 0004767, 0000016, 0002004, 0105700, 0001731, 
	0000000, 0000727, 0110321, 0000767, 0112403, 0005204, 0060300, 0042703, 
	0177400, 0005366, 0000002, 0001402, 0005302, 0000207, 0105237, 0177521, 
	0123766, 0177521, 0000006, 0001024, 0006204, 0103413, 0113737, 0177521, 
	0177522, 0162737, 0000020, 0177522, 0113737, 0177522, 0177521, 0000261, 
	0000410, 0113766, 0177521, 0000006, 0066666, 0000004, 0000006, 0000401, 
	0006204, 0012704, 0173400, 0103001, 0005204, 0012766, 0000176, 0000002, 
	0132737, 0000001, 0177521, 0001003, 0062766, 0000002, 0000002, 0000724, 
	0105700, 0001403, 0000000, 0000167, 0177406, 0032705, 0000001, 0001403, 
	0000000, 0000167, 0177372, 0112737, 0000037, 0177524, 0110137, 0177520, 
	0022705, 0173000, 0101403, 0012704, 0000400, 0000555, 0000115, 0012702, 
	0177524, 0112712, 0000005, 0012706, 0001000, 0005003, 0012701, 0000004, 
	0012721, 0173504, 0012711, 0000340, 0011313, 0062703, 0004000, 0022703, 
	0170000, 0001372, 0162703, 0000006, 0010306, 0000167, 0177266, 0000020, 
	0000040, 0000200, 0005001, 0005003, 0012706, 0001000, 0012702, 0000004, 
	0012722, 0173566, 0012712, 0000340, 0011313, 0005201, 0062703, 0004000, 
	0022703, 0170000, 0001371, 0005743, 0010306, 0000301, 0062701, 0173001, 
	0100375, 0062701, 0035057, 0012737, 0000006, 0000004, 0005037, 0000006, 
	0012705, 0173324, 0012704, 0003406, 0000167, 0000132, 0112737, 0000015, 
	0177524, 0012706, 0001000, 0005000, 0005003, 0012702, 0000004, 0012722, 
	0173702, 0012712, 0000340, 0005013, 0062703, 0004000, 0022703, 0170000, 
	0001372, 0005043, 0012705, 0173000, 0012704, 0001402, 0000167, 0000046, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 0000000, 
	0000000, 0000000, 0000000, 0010437, 0177520, 0010507, 0052525, 0044734, 
};
