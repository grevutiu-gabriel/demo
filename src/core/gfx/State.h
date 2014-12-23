#pragma once












namespace base
{


struct State
{
//	void setBlendUnchanged();
//	void setBlendEnabled();
//	void setBlendDisabled();

//private:
	enum GLState
	{
		EUnchanged = 0,
		EEnabled,
		EDisabled
	};

	State()
	{
		blend = EUnchanged;
	}




	GLState blend;
	int blendFuncSrcFactor;
	int blendFuncDstFactor;

};






}
