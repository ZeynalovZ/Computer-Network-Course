using System;
using System.Net;
using System.IO;
using System.Net.Mail;
using System.Collections.Generic;

namespace NetConsoleApp
{

    class Program
    {

        static List<string> Files = new List<string>();

        public static void MailSender(MailMessage m)
        {
            SmtpClient smtp = new SmtpClient("smtp.mail.ru", 25);

            smtp.Credentials = new NetworkCredential("zeynal.zeynalov.00@mail.ru", "password");
            smtp.EnableSsl = true;
            smtp.Send(m);
        }

        public static void FindfilesWithKey(string key)
        {
            
            string[] fileLines = { };
            
            string path = Directory.GetCurrentDirectory() + "\\";
            DirectoryInfo dir = new DirectoryInfo(path);
            foreach (var item in dir.GetFiles())
            {
               
                if (item.Name.Contains(".txt"))
                {
                    
                    fileLines = File.ReadAllLines(item.Name);
                    foreach (var line in fileLines)
                    {
                        if (line.Contains(key))
                        {
                            Files.Add(item.Name);
                        }
                    }

                }
            }
        }

        static void Main(string[] args)
        {

            MailAddress from = new MailAddress("zeynal.zeynalov.00@mail.ru", "Zeynal");
            MailAddress to = new MailAddress("zeynal.zeynalov.00@mail.ru");

            MailMessage m = new MailMessage(from, to);
            m.Subject = "Тест";
            m.Body = "<h2>Письмо-тест работы smtp-клиента</h2>";
            m.IsBodyHtml = true;


            Console.WriteLine("Введите ключевое слово:");
            var key = Console.ReadLine();
            Console.WriteLine();
            FindfilesWithKey(key);
            if (Files.Count != 0)
            {
                
                foreach (var file in Files)
                {
                    Console.WriteLine("File with key: " + file);
                    m.Attachments.Add(new Attachment(file));
                }
            }
            
            MailSender(m);
        }
    }
}