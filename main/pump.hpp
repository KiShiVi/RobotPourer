#ifndef PUMP_H
#define PUMP_H

#include "settings.hpp"
#include "timer.hpp"

/*
*	class Pump - Класс для работы с помпой
*/

class Pump
{
	//!< Класс для работы с помпой
public:
	Pump( int pin );											//!< Конструктор. pin - Пин питания на помпу
	void pumpStart( uint32_t milli = DEFAULT_FINISH_TIMER );	//!< Включить помпу на milli времени (мс) (срабатывает с задержкой, указанной в Settings)
	void pumpStartWithoutTimer();								//!< Включить помпу без задержек и таймера
	void pumpStop(bool justNow=false);							//!< Выключить помпу
	uint8_t getState();					                        //!< Получить состояние помпы (BUSY, READY)
	bool isPouring();                                         	//!< Возвращает состояние - наливает сейчас помпа или нет
	bool isFinishedPoured();                                  	//!< Возвращает состояние - разлила ли уже помпа или нет

	void pumpCheck();											//!< Метод, который следует положить в loop(). Нужен для своевременного отключения по таймеру
    
private:
	Timer * startDelayTimer;              //!< Таймер задержки перед разливом
	Timer * finishTimer;                  //!< Таймер работы помпы
	Timer * finishDelayTimer;             //!< Таймер задержки после разлива
	int m_pompPin;                        //!< Пин помпы
	enum{ BUSY = 0, READY = 1 } m_state;  //!< Состояние помпы
};

Pump::Pump( int pin )
{
	startDelayTimer		= new Timer( DEFAULT_DELAY_TIMER );
	finishTimer       	= new Timer( DEFAULT_FINISH_TIMER );
	finishDelayTimer  	= new Timer( DEFAULT_DELAY_TIMER );
	m_state = READY; 
	m_pompPin = pin;
	Serial.println(F("Pump init"));
	
	finishTimer->stop();
	startDelayTimer->stop();
	finishDelayTimer->stop();
}

void Pump::pumpStart( uint32_t milli )
{
	if ( m_state == BUSY )
	{
		Serial.println(F("Pump BUSY"));
		return;
	}
	m_state = BUSY;
	startDelayTimer->start();
	finishTimer->stop();
	finishDelayTimer->stop();
	finishTimer->setInterval( milli );
	Serial.println(F("Pump PRE-START"));
}

void Pump::pumpStartWithoutTimer()
{
	digitalWrite( m_pompPin, HIGH );
	m_state = BUSY;
}

void Pump::pumpStop( bool justNow )
{
	digitalWrite( m_pompPin, LOW );
	Serial.println(F("Pump stoped"));
	
	finishTimer->stop();
	startDelayTimer->stop();
	finishDelayTimer->start();

	if ( justNow )
	{
		finishDelayTimer->stop();
		m_state = READY;
	}
}

uint8_t Pump::getState()
{
    return m_state;
}

bool Pump::isPouring()
{
	if ( startDelayTimer->getState() == false )
		return true;
	return false;
}

bool Pump::isFinishedPoured()
{
	if ( finishTimer->getState() == false )
		return true;
	return false;
}

void Pump::pumpCheck()
{
	if ( startDelayTimer->isReady() )
	{
		startDelayTimer->stop();
		finishTimer->start();
		finishDelayTimer->stop();

		digitalWrite( m_pompPin, HIGH );
		Serial.println(F("Pump started"));
	}

	if ( finishTimer->isReady() )
		pumpStop();

	if ( finishDelayTimer->isReady() )
	{
		startDelayTimer->stop();
		finishTimer->stop();
		finishDelayTimer->stop();
		m_state = READY;
	}
}

#endif
