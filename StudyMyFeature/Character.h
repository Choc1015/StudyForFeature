#pragma once


class Character
{
public: 
	int count = 0;

	Character();
	virtual ~Character();

	virtual void Attack() const;



};
