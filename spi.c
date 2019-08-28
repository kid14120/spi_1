#include "spi.h"


volatile uint32_t Spi_cs_port;
volatile uint8_t Spi_cs_pin;


void spi_init()		//SPI0初始化
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA3_SSI0FSS);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);

	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);

	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, SSI0_BitRate, 8);
	SSIEnable(SSI0_BASE);
	//spi_set_cspin(GPIO_PORTA_BASE,GPIO_PIN_3);
}

void spi_set_cspin(uint32_t port, uint8_t pin)
{
	while(SSIBusy(SSI0_BASE));		//等待当前发送完成
	
	Spi_cs_port = port;
	Spi_cs_pin = pin;
}

void spi_cs_enable()
{
	 GPIOPinWrite(Spi_cs_port, Spi_cs_pin, 0);
}

void spi_cs_disable()
{
	 GPIOPinWrite(Spi_cs_port, Spi_cs_pin, Spi_cs_pin);
}

uint8_t spi_rw(uint8_t data)
{
	uint32_t ui32Data;
	uint8_t ui8Data;

	SSIDataPut(SSI0_BASE, data);
	while(SSIBusy(SSI0_BASE));
	
	SSIDataGet(SSI0_BASE, &ui32Data);
	ui8Data = ui32Data & 0xff;
	while(SSIBusy(SSI0_BASE));
	
	return(ui8Data);
}

void spi_write_byte(uint8_t data)
{
	uint32_t ui32Data;
	
	spi_cs_enable();
	
	SSIDataPut(SSI0_BASE, data);
	while(SSIBusy(SSI0_BASE));
	
	SSIDataGet(SSI0_BASE, &ui32Data);
	while(SSIBusy(SSI0_BASE));
	
	spi_cs_disable();
}

void spi_write_reg(uint8_t reg, uint8_t data)
{
	spi_cs_enable();
	spi_rw(reg);
	spi_rw(data);
	spi_cs_disable();
}

uint8_t spi_read_reg(uint8_t reg)
{
	uint8_t data;

	spi_cs_enable();
	spi_rw(reg);
	data = spi_rw(0);
	spi_cs_disable();
	
	return data;
}

//在指定位置写指定长度的数据
//reg:寄存器(位置)
//*pBuf:数据指针
//len:数据长度
void spi_write_buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t i;

	spi_cs_enable();
	spi_rw(reg);
	for(i = 0; i < len; i++)
	{
		spi_rw(*pBuf++);
	}
	spi_cs_disable();
}

void spi_read_buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
	uint8_t i;

	spi_cs_enable();
	
	spi_rw(reg);
	for(i = 0; i < len; i++)
	{
		pBuf[i] = spi_rw(0xff);
	}
	spi_cs_disable();
}



