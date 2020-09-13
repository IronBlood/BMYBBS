#ifndef BMYBBS_IPHASH_H
#define BMYBBS_IPHASH_H

/**
 * ���� IP ����ɢ��ֵ
 *
 * �����ٶ� IP ҪôΪ IPv4���� '.' �ָ�����Ҫô IPv6���� ':' �ָ�����IPv4 ӳ�䵽 IPv6 ��
 * ��Ϊ���ߣ������ϼ���Ӧ�ü��ݣ�������ǰ�ߣ������� nju09 �б�ע�͵���ԭ�ȵ� hash �㷨��
 * ������ IPv4 ��ַ��Ϊ 32 ����������ģ���㣬��������������ࡣ
 * @param ip     [char *] IP �ַ���
 * @param nhash  [uint32] ģ
 * @return
 */
unsigned int bmy_iphash(const char *ip, unsigned int nhash);
#endif //BMYBBS_IPHASH_H
