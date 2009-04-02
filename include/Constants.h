#ifndef BV3D_CONSTANTS
#define BV3D_CONSTANTS

//#include <vrs/vector.h>
#include <Ogre.h> //TODO: remove

namespace BV3D
{
		enum TEAM { TEAM1, TEAM2 };
		
		//enum BV3D_ERROR	{
		//	NO_ERROR,
		//	INVALID_PARAMETER
		//};

		enum CAMERAPOSITION	{
			CLASSIC_CAMERA,
			TEAM1_BASECAMERA,
			REVERSE_CAMERA,
			TEAM2_BASECAMERA,
			MAX_CAMERAS
		};

		static const Vector3 ARENA_EXTENT = Vector3(16.0,30.0,7.0);
		static const double	NET_HEIGHT = 4.2;
		static const double LINES_WIDTH = 0.2;
		static const double BALL_RADIUS = 0.9;
		static const double BLOBB_HEIGHT = 2.6;
		static const double BLOBB_ALPHA = 0.7;
		static const double BLOBB_JUMP_SPEED = 14.0;
		static const double BLOBB_MAX_JUMP_HEIGHT = 2.0;
		static const double BLOBB_MOVEMENT_STEP_DISTANCE = 12.0;	// blobb step distance
		static const double BLOBB_GRAVITY_MULTIPLIER = 3.5;			// the GRAVITY for the blobbs is stronger, because they should fall down faster than the ball
		//static const std::string MODELS_PATH = "../data/models/";
		//static const std::string GRAFICS_PATH = "../data/grafics/";
		//static const std::string SOUNDS_PATH = "../data/sounds/";
		static const Vector3 LOOK_TO = Vector3(0.0, 4.0, 0.0);
		static const Vector3 LOOK_FROM = Vector3(0.0, 9.0, -16.0);
		static const float MAX_BALL_VELOCITY = 20.0;
		static const int MOUSE_MAX_DELTA = 20;	// maximum accepted mouse movement in pixels (see MouseControls)

		static const double SHADOW_HEIGHT = 0.01;
		static const double SHADOW_MAX_HEIGHT = 20.0;

		static const double GRAVITY = 21.0;
		static const double MAX_FPS = 32.0;

		// data per blobb:
		//	lower vertical offset/radius, lower horizontal radius
		//	upper vertical offset, upper horizontal radius, upper vertical radius
		static const float BLOBB_SHAPE_DATA[5][5] = {
			{1.0f+0.1f, 1.0f, 1.5f, 0.9f, 1.1f},
			{0.9f+0.1f, 1.01f, 1.35f, 0.9f, 1.1f},
			{0.8f+0.1f, 1.05f, 1.2f, 0.9f, 1.1f},
			{0.65f+0.1f, 1.1f, 1.1f, 0.9f, 1.0f},
			{0.5f+0.1f, 1.2f, 1.0f, 0.9f, 0.9f} };

		static const std::string CityMeshes[] = {
			"buildin100_buildz5.mesh", "buildin101_buildz5.mesh", "buildin102_buildz5.mesh", "buildin103_skyscrp.mesh", "buildin104_skyscrp.mesh", "buildin105_skyscrp.mesh", "buildin106_skyscrp.mesh", "buildin107_skyscrp.mesh", "buildin108_buildz4_bl.mesh", "buildin109_buildz4_bl.mesh", 
			"buildin110_skyscrp.mesh", "buildin111_buildz4_bl.mesh", "buildin112_buildz4_bl.mesh", "buildin113_skyscrp.mesh", "buildin114_skyscrp.mesh", "buildin115_buildz4_bl.mesh", "buildin116_skyscrp.mesh", "building10_skyscrp.mesh", "building11_skyscrp.mesh", "building12_skyscrp.mesh", 
			"building13_skyscrp.mesh", "building14_skyscrp.mesh", "building15_skyscrp.mesh", "building16_buildz2_bl.mesh", "building17_skyscrp.mesh", "building18_skyscrp.mesh", "building19_skyscrp.mesh", "building20_skyscrp.mesh", "building21_buildz1_bl.mesh", "building22_skyscrp.mesh", 
			"building23_buildz4_bl.mesh", "building24_buildz4_bl.mesh", "building25_skyscrp.mesh", "building26_buildz4_bl.mesh", "building27_buildz4_bl.mesh", "building28_buildz4_bl.mesh", "building29_buildz4_bl.mesh", "building30_buildz4_bl.mesh", "building31_buildz4_bl.mesh", "building32_buildz4_bl.mesh", 
			"building33_buildz4_bl.mesh", "building34_buildz4_bl.mesh", "building35_buildz4_bl.mesh", "building36_buildz4_bl.mesh", "building37_buildz4_bl.mesh", "building38_buildz4_bl.mesh", "building39_buildz4_bl.mesh", "building40_buildz4_bl.mesh", "building41_buildz4_bl.mesh", "building42_buildz4_bl.mesh", 
			"building43_skyscrp.mesh", "building44_skyscrp.mesh", "building45_buildz4_bl.mesh", "building46_buildz4_bl.mesh", "building47_buildz1_bl.mesh", "building48_buildz4_bl.mesh", "building49_buildz4_bl.mesh", "building50_skyscrp.mesh", "building51_buildz4_bl.mesh", "building52_buildz4_bl.mesh", 
			"building53_buildz4_bl.mesh", "building54_buildz4_bl.mesh", "building55_buildz4_bl.mesh", "building56_buildz4_bl.mesh", "building57_buildz4_bl.mesh", "building58_buildz6.mesh", "building59_buildz6.mesh", "building60_buildz1_bl.mesh", "building61_buildz1_bl.mesh", "building62_buildz1_bl.mesh", 
			"building63_buildz6.mesh", "building64_buildz5.mesh", "building65_buildz5.mesh", "building66_buildz5.mesh", "building67_buildz5.mesh", "building68_buildz5.mesh", "building69_buildz5.mesh", "building70_buildz5.mesh", "building71_buildz5.mesh", "building72_buildz5.mesh", 
			"building73_buildz5.mesh", "building74_buildz5.mesh", "building75_buildz5.mesh", "building76_buildz5.mesh", "building77_buildz3_bl.mesh", "building78_skyscrp.mesh", "building79_skyscrp.mesh", "building80_skyscrp.mesh", "building81_buildz3_bl.mesh", "building82_buildz6.mesh", 
			"building83_buildz1_bl.mesh", "building84_buildz6.mesh", "building85_skyscrp.mesh", "building86_skyscrp.mesh", "building87_buildz2_bl.mesh", "building88_buildz5.mesh", "building89_buildz5.mesh", "building90_buildz5.mesh", "building91_buildz5.mesh", "building92_buildz5.mesh", 
			"building93_buildz5.mesh", "building94_buildz5.mesh", "building95_buildz5.mesh", "building96_buildz5.mesh", "building97_buildz5.mesh", "building98_buildz5.mesh", "building99_buildz5.mesh", "building_0_buildz5.mesh", "building_1_buildz5.mesh", "building_2_buildz5.mesh", 
			"building_3_buildz5.mesh", "building_4_buildz4_bl.mesh", "building_5_skyscrp.mesh", "building_6_skyscrp.mesh", "building_7_skyscrp.mesh", "building_8_skyscrp.mesh", "building_9_skyscrp.mesh", "building_c_buildz4_bl.mesh", "building_h_buildz4_bl.mesh", "building_i_buildz5.mesh", 
			"building_m_buildz4_bl.mesh", "building_r_buildz6.mesh", "building_s_buildz4_bl.mesh", "Garage_19_buildz6.mesh", "Garage_23_buildz6.mesh", "Garage_25_buildz6.mesh", "Garage_28_buildz6.mesh", "Garage_29_buildz6.mesh", "gas_statio_buildz4_bl.mesh", "Hospital04_skyscrp.mesh", 
			"Hospital05_skyscrp.mesh", "Hospital07_buildz4_bl.mesh", "Hospital09_buildz4_bl.mesh", "Hospital10_buildz4_bl.mesh", "house_180_buildz1_blu.mesh", "house_181_buildz1_blu.mesh", "house_182_buildz5.mesh", "house_183_buildz2_blu.mesh", "house_186_buildz3_blu.mesh", "house_187_buildz3_blu.mesh", 
			"house_189_buildz3_blu.mesh", "house_190_buildz1_blu.mesh", "house_191_buildz1_blu.mesh", "house_192_buildz1_blu.mesh", "house_193_buildz3_blu.mesh", "house_194_buildz3_blu.mesh", "house_195_buildz3_blu.mesh", "house_196_buildz3_blu.mesh", "house_197_buildz3_blu.mesh", "house_198_buildz3_blu.mesh", 
			"house_199_buildz3_blu.mesh", "house_200_buildz1_blu.mesh", "house_201_buildz1_blu.mesh", "house_202_buildz3_blu.mesh", "house_204_buildz2_blu.mesh", "house_206_buildz2_blu.mesh", "house_208_buildz2_blu.mesh", "house_209_buildz2_blu.mesh", "house_210_buildz2_blu.mesh", "house_211_buildz2_blu.mesh", 
			"house_212_buildz2_blu.mesh", "house_213_buildz2_blu.mesh", "house_215_buildz3_blu.mesh", "house_216_buildz3_blu.mesh", "house_217_buildz3_blu.mesh", "house_218_buildz5.mesh", "house_219_buildz5.mesh", "house_220_buildz6.mesh", "house_221_buildz6.mesh", "house_222_buildz6.mesh", 
			"house_223_buildz5.mesh", "house_224_buildz5.mesh", "house_225_buildz5.mesh", "house_226_buildz6.mesh", "house_227_buildz6.mesh", "house_228_buildz6.mesh", "house_229_buildz2_blu.mesh", "house_230_buildz2_blu.mesh", "house_231_buildz2_blu.mesh", "house_232_buildz2_blu.mesh", 
			"house_234_buildz2_blu.mesh", "house_235_buildz2_blu.mesh", "house_236_buildz5.mesh", "house_237_buildz5.mesh", "house_238_buildz5.mesh", "house_239_buildz5.mesh", "house_240_buildz5.mesh", "house_241_buildz1_blu.mesh", "house_242_buildz1_blu.mesh", "house_243_buildz1_blu.mesh", 
			"house_244_buildz1_blu.mesh", "house_245_buildz1_blu.mesh", "house_246_buildz5.mesh", "house_247_buildz1_blu.mesh", "house_248_buildz1_blu.mesh", "house_249_buildz1_blu.mesh", "house_251_buildz3_blu.mesh", "house_252_buildz5.mesh", "house_253_buildz3_blu.mesh", "house_254_buildz3_blu.mesh", 
			"house_255_buildz3_blu.mesh", "house_256_buildz3_blu.mesh", "house_257_buildz1_blu.mesh", "house_258_buildz3_blu.mesh", "house_259_buildz2_blu.mesh", "house_260_buildz3_blu.mesh", "house_262_buildz1_blu.mesh", "house_263_buildz1_blu.mesh", "house_264_buildz3_blu.mesh", "house_265_buildz3_blu.mesh", 
			"house_266_buildz1_blu.mesh", "house_267_buildz6.mesh", "house_268_buildz6.mesh", "house_269_buildz6.mesh", "house_270_buildz6.mesh", "house_271_buildz5.mesh", "house_272_buildz5.mesh", "house_273_buildz5.mesh", "house_274_buildz5.mesh", "house_275_buildz1_blu.mesh", 
			"house_276_buildz1_blu.mesh", "house_277_buildz1_blu.mesh", "house_280_buildz3_blu.mesh", "house_281_buildz3_blu.mesh", "house_282_buildz1_blu.mesh", "house_283_buildz1_blu.mesh", "house_284_buildz1_blu.mesh", "house_285_buildz1_blu.mesh", "house_286_buildz3_blu.mesh", "house_287_buildz3_blu.mesh", 
			"house_289_buildz1_blu.mesh", "house_290_buildz1_blu.mesh", "house_291_buildz1_blu.mesh", "house_292_buildz5.mesh", "house_293_buildz1_blu.mesh", "house_294_buildz1_blu.mesh", "house_295_buildz1_blu.mesh", "house_297_buildz3_blu.mesh", "house_298_buildz1_blu.mesh", "house_299_buildz1_blu.mesh", 
			"house_300_buildz1_blu.mesh", "house_301_buildz3_blu.mesh", "house_302_buildz6.mesh", "house_303_buildz1_blu.mesh", "house_304_buildz3_blu.mesh", "house_305_buildz6.mesh", "house_306_buildz6.mesh", "house_307_buildz3_blu.mesh", "house_308_buildz3_blu.mesh", "house_309_buildz3_blu.mesh", 
			"house_310_buildz3_blu.mesh", "house_311_buildz3_blu.mesh", "house_312_buildz5.mesh", "house_313_buildz1_blu.mesh", "house_314_buildz1_blu.mesh", "house_315_buildz2_blu.mesh", "house_317_buildz6.mesh", "house_318_buildz5.mesh", "house_319_buildz2_blu.mesh", "house_320_buildz2_blu.mesh", 
			"house_321_buildz6.mesh", "house_322_buildz3_blu.mesh", "House_38_buildz4_blur.mesh", "House_39_buildz4_blur.mesh", "House_40_buildz4_blur.mesh", "House_41_buildz4_blur.mesh", "House_64_buildz4_blur.mesh", "House_65_buildz4_blur.mesh", "House_66_buildz4_blur.mesh", "House_67_buildz4_blur.mesh", 
			"House_68_buildz4_blur.mesh", "House_69_buildz4_blur.mesh", "House_70_buildz4_blur.mesh", "House_71_buildz4_blur.mesh", "House_72_buildz4_blur.mesh", "House_73_buildz4_blur.mesh", "misc_kios0_buildz4_bl.mesh", "misc_kios1_buildz4_bl.mesh", "misc_kios2_buildz4_bl.mesh", "misc_kios3_buildz4_bl.mesh", 
			"misc_kiosk_buildz4_bl.mesh", "misc_la100_roads_blur.mesh", "misc_la101_roads_blur.mesh", "misc_la102_roads_blur.mesh", "misc_la103_roads_blur.mesh", "misc_la104_roads_blur.mesh", "misc_la105_roads_blur.mesh", "misc_la106_roads_blur.mesh", "misc_la107_roads_blur.mesh", "misc_la108_roads_blur.mesh", 
			"misc_la109_roads_blur.mesh", "misc_la110_roads_blur.mesh", "misc_la111_roads_blur.mesh", "misc_la112_roads_blur.mesh", "misc_la113_roads_blur.mesh", "misc_la114_roads_blur.mesh", "misc_la115_roads_blur.mesh", "misc_la116_roads_blur.mesh", "misc_la117_roads_blur.mesh", "misc_la118_roads_blur.mesh", 
			"misc_la119_roads_blur.mesh", "misc_la120_roads_blur.mesh", "misc_la121_roads_blur.mesh", "misc_la122_roads_blur.mesh", "misc_la123_roads_blur.mesh", "misc_la124_roads_blur.mesh", "misc_la125_roads_blur.mesh", "misc_la126_roads_blur.mesh", "misc_la127_roads_blur.mesh", "misc_la128_roads_blur.mesh", 
			"misc_la129_roads_blur.mesh", "misc_la130_roads_blur.mesh", "misc_la131_roads_blur.mesh", "misc_la132_roads_blur.mesh", "misc_la133_roads_blur.mesh", "misc_la134_roads_blur.mesh", "misc_la135_roads_blur.mesh", "misc_la136_roads_blur.mesh", "misc_la137_roads_blur.mesh", "misc_la138_roads_blur.mesh", 
			"misc_la139_roads_blur.mesh", "misc_la140_roads_blur.mesh", "misc_la141_roads_blur.mesh", "misc_la142_roads_blur.mesh", "misc_la143_roads_blur.mesh", "misc_la144_roads_blur.mesh", "misc_la145_roads_blur.mesh", "misc_la146_roads_blur.mesh", "misc_la147_roads_blur.mesh", "misc_la148_roads_blur.mesh", 
			"misc_lam10_roads_blur.mesh", "misc_lam11_roads_blur.mesh", "misc_lam12_roads_blur.mesh", "misc_lam13_roads_blur.mesh", "misc_lam14_roads_blur.mesh", "misc_lam15_roads_blur.mesh", "misc_lam16_roads_blur.mesh", "misc_lam17_roads_blur.mesh", "misc_lam18_roads_blur.mesh", "misc_lam19_roads_blur.mesh", 
			"misc_lam20_roads_blur.mesh", "misc_lam21_roads_blur.mesh", "misc_lam22_roads_blur.mesh", "misc_lam23_roads_blur.mesh", "misc_lam24_roads_blur.mesh", "misc_lam25_roads_blur.mesh", "misc_lam26_roads_blur.mesh", "misc_lam27_roads_blur.mesh", "misc_lam28_roads_blur.mesh", "misc_lam29_roads_blur.mesh", 
			"misc_lam30_roads_blur.mesh", "misc_lam31_roads_blur.mesh", "misc_lam32_roads_blur.mesh", "misc_lam33_roads_blur.mesh", "misc_lam34_roads_blur.mesh", "misc_lam35_roads_blur.mesh", "misc_lam36_roads_blur.mesh", "misc_lam37_roads_blur.mesh", "misc_lam38_roads_blur.mesh", "misc_lam39_roads_blur.mesh", 
			"misc_lam40_roads_blur.mesh", "misc_lam41_roads_blur.mesh", "misc_lam42_roads_blur.mesh", "misc_lam43_roads_blur.mesh", "misc_lam44_roads_blur.mesh", "misc_lam45_roads_blur.mesh", "misc_lam46_roads_blur.mesh", "misc_lam47_roads_blur.mesh", "misc_lam48_roads_blur.mesh", "misc_lam49_roads_blur.mesh", 
			"misc_lam50_roads_blur.mesh", "misc_lam51_roads_blur.mesh", "misc_lam52_roads_blur.mesh", "misc_lam53_roads_blur.mesh", "misc_lam54_roads_blur.mesh", "misc_lam55_roads_blur.mesh", "misc_lam56_roads_blur.mesh", "misc_lam57_roads_blur.mesh", "misc_lam58_roads_blur.mesh", "misc_lam59_roads_blur.mesh", 
			"misc_lam60_roads_blur.mesh", "misc_lam61_roads_blur.mesh", "misc_lam62_roads_blur.mesh", "misc_lam63_roads_blur.mesh", "misc_lam64_roads_blur.mesh", "misc_lam65_roads_blur.mesh", "misc_lam66_roads_blur.mesh", "misc_lam67_roads_blur.mesh", "misc_lam68_roads_blur.mesh", "misc_lam69_roads_blur.mesh", 
			"misc_lam70_roads_blur.mesh", "misc_lam71_roads_blur.mesh", "misc_lam72_roads_blur.mesh", "misc_lam73_roads_blur.mesh", "misc_lam74_roads_blur.mesh", "misc_lam75_roads_blur.mesh", "misc_lam76_roads_blur.mesh", "misc_lam77_roads_blur.mesh", "misc_lam78_roads_blur.mesh", "misc_lam79_roads_blur.mesh", 
			"misc_lam80_roads_blur.mesh", "misc_lam81_roads_blur.mesh", "misc_lam82_roads_blur.mesh", "misc_lam83_roads_blur.mesh", "misc_lam84_roads_blur.mesh", "misc_lam85_roads_blur.mesh", "misc_lam86_roads_blur.mesh", "misc_lam87_roads_blur.mesh", "misc_lam88_roads_blur.mesh", "misc_lam89_roads_blur.mesh", 
			"misc_lam90_roads_blur.mesh", "misc_lam91_roads_blur.mesh", "misc_lam92_roads_blur.mesh", "misc_lam93_roads_blur.mesh", "misc_lam94_roads_blur.mesh", "misc_lam95_roads_blur.mesh", "misc_lam96_roads_blur.mesh", "misc_lam97_roads_blur.mesh", "misc_lam98_roads_blur.mesh", "misc_lam99_roads_blur.mesh", 
			"misc_lamp0_roads_blur.mesh", "misc_lamp1_roads_blur.mesh", "misc_lamp2_roads_blur.mesh", "misc_lamp3_roads_blur.mesh", "misc_lamp4_roads_blur.mesh", "misc_lamp5_roads_blur.mesh", "misc_lamp6_roads_blur.mesh", "misc_lamp7_roads_blur.mesh", "misc_lamp8_roads_blur.mesh", "misc_lamp9_roads_blur.mesh", 
			"misc_lampp_roads_blur.mesh", "misc_roa10_roads_blur.mesh", "misc_road0_roads_blur.mesh", "misc_road1_roads_blur.mesh", "misc_road2_roads_blur.mesh", "misc_road3_roads_blur.mesh", "misc_road4_roads_blur.mesh", "misc_road5_roads_blur.mesh", "misc_road6_roads_blur.mesh", "misc_road7_roads_blur.mesh", 
			"misc_road8_roads_blur.mesh", "misc_road9_roads_blur.mesh", "misc_roads_roads_blur.mesh", "misc_tra10_roads_blur.mesh", "misc_tra11_roads_blur.mesh", "misc_tra12_roads_blur.mesh", "misc_tra13_roads_blur.mesh", "misc_tra14_roads_blur.mesh", "misc_tra15_roads_blur.mesh", "misc_tra16_roads_blur.mesh", 
			"misc_tra17_roads_blur.mesh", "misc_tra18_roads_blur.mesh", "misc_tra19_roads_blur.mesh", "misc_tra20_roads_blur.mesh", "misc_traf0_roads_blur.mesh", "misc_traf1_roads_blur.mesh", "misc_traf2_roads_blur.mesh", "misc_traf3_roads_blur.mesh", "misc_traf4_roads_blur.mesh", "misc_traf5_roads_blur.mesh", 
			"misc_traf6_roads_blur.mesh", "misc_traf7_roads_blur.mesh", "misc_traf8_roads_blur.mesh", "misc_traf9_roads_blur.mesh", "misc_traff_roads_blur.mesh", "Object01_buildz4_blur.mesh", "Object02_skyscrp.mesh", "Object03_buildz4_blur.mesh", "Object04_buildz4_blur.mesh", "Object05_buildz4_blur.mesh", 
			"Object06_buildz4_blur.mesh", "Object07_buildz4_blur.mesh", "Object08_buildz4_blur.mesh", "Object09_buildz2_blur.mesh", "Object10_skyscrp.mesh", "Object11_skyscrp.mesh", "Object12_skyscrp.mesh", "Object13_skyscrp.mesh", "Object14_skyscrp.mesh", "Object15_skyscrp.mesh", 
			"Object18_skyscrp.mesh", "Object19_skyscrp.mesh", "park_baske_buildz3.mesh", "park_baske_grass.mesh", "park_baske_grass_2.mesh", "park_baske_roads_2_bl.mesh", "park_baske_roads_blur.mesh", "park_baske_sand_01_bl.mesh", "sky_scra10_skyscrp.mesh", "sky_scra11_skyscrp.mesh", 
			"sky_scra12_skyscrp.mesh", "sky_scra13_skyscrp.mesh", "sky_scra14_skyscrp.mesh", "sky_scra15_skyscrp.mesh", "sky_scra16_skyscrp.mesh", "sky_scra17_skyscrp.mesh", "sky_scra18_skyscrp.mesh", "sky_scra19_skyscrp.mesh", "sky_scra20_skyscrp.mesh", "sky_scra21_skyscrp.mesh", 
			"sky_scra22_skyscrp.mesh", "sky_scra23_skyscrp.mesh", "sky_scrap0_skyscrp.mesh", "sky_scrap1_skyscrp.mesh", "sky_scrap2_skyscrp.mesh", "sky_scrap3_skyscrp.mesh", "sky_scrap4_skyscrp.mesh", "sky_scrap5_skyscrp.mesh", "sky_scrap6_skyscrp.mesh", "sky_scrap7_skyscrp.mesh", 
			"sky_scrap8_skyscrp.mesh", "sky_scrap9_skyscrp.mesh", "sky_scrape_skyscrp.mesh", "substruc10_buildz3.mesh", "substruc10_grass_2.mesh", "substruc11_buildz3.mesh", "substruc11_grass_2.mesh", "substruc12_buildz3.mesh", "substruc12_grass_2.mesh", "substruc13_buildz3.mesh", 
			"substruc13_grass_2.mesh", "substruc14_buildz3.mesh", "substruc14_grass_2.mesh", "substruc15_buildz3.mesh", "substruc15_grass_2.mesh", "substruc16_buildz3.mesh", "substruc16_grass_2.mesh", "substruc17_buildz3.mesh", "substruc17_grass_2.mesh", "substruc18_buildz3.mesh", 
			"substruc18_grass_2.mesh", "substruc19_buildz3.mesh", "substruc19_grass_2.mesh", "substruc20_buildz3.mesh", "substruc20_grass_2.mesh", "substruc21_buildz3.mesh", "substruc21_grass_2.mesh", "substruc22_buildz3.mesh", "substruc22_grass_2.mesh", "substruc23_buildz3.mesh", 
			"substruc23_grass_2.mesh", "substruc24_buildz3.mesh", "substruc24_grass_2.mesh", "substruc25_buildz3.mesh", "substruc25_grass_2.mesh", "substruc26_buildz3.mesh", "substruc26_grass_2.mesh", "substruc27_buildz3.mesh", "substruc27_grass_2.mesh", "substruc28_buildz3.mesh", 
			"substruc28_grass_2.mesh", "substruct0_buildz3.mesh", "substruct0_grass_2.mesh", "substruct1_buildz3.mesh", "substruct1_grass_2.mesh", "substruct2_buildz3.mesh", "substruct2_grass_2.mesh", "substruct3_buildz3.mesh", "substruct3_grass_2.mesh", "substruct4_buildz3.mesh", 
			"substruct4_grass_2.mesh", "substruct5_buildz3.mesh", "substruct5_grass_2.mesh", "substruct6_buildz3.mesh", "substruct6_grass_2.mesh", "substruct7_buildz3.mesh", "substruct7_grass_2.mesh", "substruct8_buildz3.mesh", "substruct8_grass_2.mesh", "substruct9_buildz3.mesh", 
			"substruct9_grass_2.mesh", "substructu_buildz3.mesh", "substructu_grass_2.mesh", "terrain_buildz3.mesh", "terrain_grass.mesh", "terrain_grass_2.mesh", "terrain_roads_2_blur_.mesh", "terrain_roads_blur_.mesh", "terrain_sand_01_blur_.mesh", "water_ghos_water.mesh"
		};
};

#endif	// BV3D_CONSTANTS
